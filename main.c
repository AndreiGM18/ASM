#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include "structs.h"

#define MAX_LINE_SIZE 256

#define uint unsigned int
#define uchar unsigned char

// Useful macro for handling errors
#define DIE(assertion, call_description)            \
	do                                              \
	{                                               \
		if (assertion)                              \
		{                                           \
			fprintf(stderr, "(%s, %d): ", __FILE__, \
					__LINE__);                      \
			perror(call_description);               \
			exit(errno);                            \
		}                                           \
	} while (0)

// Checks if a string is an integer or not
int check_str_num(char *str)
{
	// The number may be negative
	if (!isdigit(str[0]) && str[0] != '-')
		return 0;

	for (long i = 1; i < strlen(str); ++i) {
		if (!isdigit(str[i]))
			return 0;
	}

	return 1;
}

// Frees the memory allocated for a data_structure struct
void free_data_s(data_structure *data_s)
{
	free(data_s->header);
	free(data_s->data);
	free(data_s);
}

// Adds a string to the data_structure's void *
void add_data_s_str(data_structure *data_s, char *tok)
{
	// Reallocates memory
	// The memory required is equal to the length of the string + 1 (for '\0')
	void *new = realloc(data_s->data, data_s->header->len +
		(strlen(tok) + 1) * sizeof(char));
	DIE(!new, "new realloc failed");
	data_s->data = new;

	// Adds the string
	memcpy(data_s->data + data_s->header->len, tok, (strlen(tok)) * sizeof(char));

	// Adds '\0' manually at the end
	*(char *)(data_s->data + data_s->header->len + strlen(tok)) = '\0'; 

	// Adds the string's length + 1 to the "len" parameter of the header
	data_s->header->len += (strlen(tok) + 1);
}

// Adds an integer to the data_structure's void *
void add_data_s_int(data_structure *data_s, char *tok, int size)
{
	// Reallocates memory
	// The memory required is equal to the "size" parameter
	void *new = realloc(data_s->data, data_s->header->len + size);
	DIE(!new, "new realloc failed");
	data_s->data = new;

	// Adds the integer
	// The type of integer is specified by the "size" parameter
	if (size == sizeof(int8_t)) {
		int8_t note = (int8_t)atoi(tok);
		memcpy(data_s->data + data_s->header->len, &note, size);
	} else if (size == sizeof(int16_t)) {
		int16_t note = (int16_t)atoi(tok);
		memcpy(data_s->data + data_s->header->len, &note, size);
	} else if (size == sizeof(int32_t)) {
		int32_t note = (int32_t)atoi(tok);
		memcpy(data_s->data + data_s->header->len, &note, size);
	}

	// Add "size" to the "len" parameter of the header
	data_s->header->len += size;
}

// Creates the data_structure, whose head and *actual* data is
// to be added to "arr"
data_structure* get_data_s(char *tok)
{
	// Allocates memory for its fields
	data_structure *data_s = (data_structure *)malloc(sizeof(data_structure));
	DIE(!data_s, "data_s malloc failed");

	data_s->header = (head *)malloc(sizeof(head));
	DIE(!data_s->header, "data_s->header malloc failed");

	data_s->data = malloc(0);
	DIE(!data_s->data, "data_s->data malloc failed");

	// Gets the type
	tok = strtok(NULL, "\n ");

	// Checking after strtok
	if (!tok || !check_str_num(tok)) {
		free_data_s(data_s);
		return NULL;
	}

	// Adds the type to the appropiate field
	// Initializes len
	data_s->header->type = *(uchar *)tok;
	data_s->header->len = (uint)0;

	// The two numbers' sizes
	int size1, size2;

	// Gets the sizes based on the specified type
	if (data_s->header->type == '1') {
		size1 = size2 = sizeof(int8_t);
	} else if (data_s->header->type == '2') {
		size1 = sizeof(int16_t);
		size2 = sizeof(int32_t);
	} else if (data_s->header->type == '3') {
		size1 = size2 = sizeof(int32_t);
	}

	// Gets the first name
	tok = strtok(NULL, "\n ");

	// Checking after strtok
	if (!tok) {
		free_data_s(data_s);
		return NULL;
	}

	// Adds it
	add_data_s_str(data_s, tok);

	// Gets the first note
	tok = strtok(NULL, "\n ");

	// Checking after strtok
	if (!tok || !check_str_num(tok)) {
		free_data_s(data_s);
		return NULL;
	}

	// Adds it
	add_data_s_int(data_s, tok, size1);

	// Gets the second note
	tok = strtok(NULL, "\n ");

	// Checking after strtok
	if (!tok || !check_str_num(tok)) {
		free_data_s(data_s);
		return NULL;
	}

	// Adds it
	add_data_s_int(data_s, tok, size2);

	// Gets the second name
	tok = strtok(NULL, "\n ");

	// Checking after strtok
	if (!tok) {
		free_data_s(data_s);
		return NULL;
	}

	// Adds it
	add_data_s_str(data_s, tok);

	// Returns the newly created data_structure
	return data_s;
}

// Gets how many bytes one must jump in order to get to an element in arr
// Can also be used to get to the end of arr
// The element's index is specified when the function is called
int get_size_till_index(void *arr, int index)
{
	// Position "it" at the start of arr
	void *it = arr;

	uint size_till_index = 0;

	// Jumps from one element to another with "it"
	// Adds how many bytes it jumped in order to get to the next one
	// An element has a size of sizeof(head) + the actual data's size
	// This is specified in the header's len field
	// When the element's header was added to arr, the "len" field was also added
	// If "it" is at the start of the header, "len" is located at "it + 4"
	// The header struct is made up of an unsigned char and an unsigned int
	// Normally, this would mean that "len" is located at "it + 1"
	// This is not the case however, as padding is applied
	// sizeof(head) is 8, with "type" and "len" both being represented on 4 bytes
	for (int i = 0; i < index; ++i) {
		size_till_index += sizeof(head) + *(uint *)(it + sizeof(uint));
		it += + sizeof(head) + *(uint *)(it + sizeof(uint));
	}
	return size_till_index;
}

// Adds the data_structure's header and *actual* data at the end of arr
// *Actual* data refers to the data that data_s->data *points* to
// It does not refer to data_s->data, which is a pointer
int add_last(void **arr, int *len, data_structure *data_s)
{
	// If data does not exist, it exits the function 
	if (!data_s)
		return -1;

	// Gets how many bytes it must jump to get to the end of arr
	uint full_size = get_size_till_index(*arr, *len);

	// Reallocates memory, so that a new element may be added to arr
	// An element is made up of its header and its *actual* data
	// As such, memory must be allocated for both
	void *new = realloc(*arr, full_size + sizeof(head) + data_s->header->len);
	DIE(!new, "new realloc failed");
	*arr = new;

	// Jumps to the end
	void *it = *arr + full_size;

	// Adds the header
	memcpy(it, data_s->header, sizeof(head));

	// Jumps over the header
	it += sizeof(head);

	// Adds the *actual* data
	memcpy(it, data_s->data, data_s->header->len);

	// Frees the data_structure struct
	free_data_s(data_s);

	// Adds 1 to len
	++(*len);

 	return 0;
}

// Adds the data_structure's header and *actual* data at a specified 
// index in arr
int add_at(void **arr, int *len, data_structure *data_s, int index)
{
	// If the index is negative or the data_structure does not exist,
	// it exits the function
	if (index < 0 || !data_s)
		return -1;

	// If the index is greater or equal to len, the element is added at the end
	if (index >= *len) {
		add_last(arr, len, data_s);
		return 0;
	}

	// Gets the number of bytes that must be jumped to get to the specified index
	uint size_till_index = get_size_till_index(*arr, index);

	// Gets the number of bytes that must be jumped to get to the end of arr
	uint full_size = get_size_till_index(*arr, *len);

	// Reallocates memory, so that a new element may be added to arr
	void *new = realloc(*arr, full_size + sizeof(head) + data_s->header->len);
	DIE(!new, "new realloc failed");
	*arr = new;

	// Jumps to the element
	void *it = *arr + size_till_index;

	// Moves all elements after index to the right in arr
	// The new element now has enough space in order to be inserted into arr
	memmove(it + sizeof(head) + data_s->header->len, it,
		(full_size - size_till_index) * sizeof(char));

	// Adds the header
	memcpy(it, data_s->header, sizeof(head));

	// Jumps over the header
	it += sizeof(head);

	// Adds the *actual* data
	memcpy(it, data_s->data, data_s->header->len);

	// Frees the data_structure struct
	free_data_s(data_s);

	// Adds 1 to len
	++(*len);

	return 0;
}

// Prints an element in arr
// The parameter is a pointer to the element's header
void print_elem(void *it)
{
	// Gets the element's type
	uchar type = *(uchar *)it;

	// Prints it
	printf("Tipul %c\n", type);

	// Jumps over the head
	it += sizeof(head);

	// Prints characters until '\0' is met
	// As such, the first name is printed
	char *c =  NULL;
	c = (char *)it;
	while (*c) {
		printf("%c", *c);
		++it;
		c = (char *)it;
	}
	// Jumps over '\0'
	++it;

	printf(" pentru ");

	// Jumps over the numbers
	if (type == '1') {
		it += 2 * sizeof(int8_t);
	} else if (type == '2') {
		it += sizeof(int16_t) + sizeof(int32_t);
	} else if (type == '3') {
		it += 2 * sizeof(int32_t);
	}

	// The length of the second name
	// It is calculated so that "it" may be able to jump back to the numbers
	int n = 0;

	// Prints characters until '\0' is met
	// As such, the second name is printed
	c = NULL;
	c = (char *)it;
	while (*c) {
		printf("%c", *c);
		++it;
		++n;
		c = (char *)it;
	}

	printf("\n");

	// Jumps back to the numbers
	it -= n;

	// Prints the numbers based on their inttype
	if (type == '1') {
		it -= 2 * sizeof(int8_t);
		printf("%"PRId8"\n%"PRId8"", *(int8_t *)it,
			*(int8_t *)(it + sizeof(int8_t)));
	} else if (type == '2') {
		it -= sizeof(int16_t) + sizeof(int32_t);
		printf("%"PRId16"\n%"PRId32"", *(int16_t *)it,
			*(int32_t *)(it + sizeof(int16_t)));
	} else if (type == '3') {
		it -= 2 * sizeof(int32_t);
		printf("%"PRId32"\n%"PRId32"", *(int32_t *)it,
			*(int32_t *)(it + sizeof(int32_t)));
	}
	printf("\n\n");
}

// Gets to an element and prints it
// The specified parameter is the element's index
void find(void *arr, int len, int index) 
{
	// If the specified index is invalid, it exits the function
	if (index < 0 && index >= len)
		return;

	// Gets how many bytes must be jumped to get to the element
	uint size_till_index = get_size_till_index(arr, index);

	// Jumps to the element
	void *it = arr + size_till_index;

	// Prints the element
	print_elem(it);
}

// Deletes an elemented from a specified index in arr
int delete_at(void **arr, int *len, int index)
{
	// If the specified index is invalid, it exits the function
	if (index < 0 && index >= *len)
		return -1;

	// Gets the number of bytes that must be jumped to get to the specified index
	uint size_till_index = get_size_till_index(*arr, index);

	// Gets the number of bytes that must be jumped to get to the end of arr
	uint full_size = get_size_till_index(*arr, *len);

	// Jumps to the element that is to be deleted
	// More specifically, to its header
	void *it = *arr + size_till_index;

	// Gets the *actual* data's size
	// It is located at "it + 4", due to the padding of the header struct
	uint data_s_len = *(int *)(it + sizeof(uint));

	// Moves all elements in arr starting at (index + 1) to the left
	// As such, the element is overridden
	memmove(it, it + sizeof(head) + data_s_len, (full_size -
		(size_till_index + sizeof(head) + data_s_len) * sizeof(char)));

	// Subtracts 1 from len
	--(*len);

	// Reallocates memory, less of it to be precise, as an element was deleted
	void *new = NULL;
	new = realloc(*arr, full_size - sizeof(head) - data_s_len);

	// After deletion, *arr may be NULL
	// If len is 0, that means that there are no more elements, and *arr should be NULL
	// If it is greater than 0, that means that realloc failed 
	DIE(!new && (*len > 0), "new realloc failed");
	*arr = new;

 	return 0;
}

// Prints all elements in arr
void print_all(void *arr, int len)
{
	// Position "it" at the start of arr
	void *it = arr;

	// Jumps to each element and prints it
	for (int i = 0; i < len; ++i) {
		print_elem(it);
		it += + sizeof(head) + *(uint *)(it + sizeof(uint));
	}
}

int main(void)
{
	// All elements are held in arr
	// len specifies how many elements are present in arr
	void *arr = NULL;
	arr = malloc(0);
	DIE(!arr, "arr malloc failed");

	int len = 0;

	// Allocates memory for a line read from stdin
	char *line = (char *)malloc(MAX_LINE_SIZE * sizeof(char));
	DIE(!line, "line malloc failed");

	// Reads multiple lines, each representing a command
	// If a command is invalid, or it does not have enough or valid
	// paramenters, nothing happens.
	// Stops when the "exit" command is given
	while (fgets(line, MAX_LINE_SIZE, stdin)) {
		int length = strlen(line);

		if (line[length - 1] == '\n')
			line[length - 1] = '\0';

		char *tok = strtok(line, "\n ");

		if (tok) {
			if (!strcmp(tok, "insert")) {
				data_structure *data_s = get_data_s(tok);

				add_last(&arr, &len, data_s);
			} else if (!strcmp(tok, "insert_at")) {
				tok = strtok(NULL, "\n ");

				if (tok || check_str_num(tok)) {
					int index = atoi(tok);

					data_structure *data_s = get_data_s(tok);

					add_at(&arr, &len, data_s, index);
				}
			} else if (!strcmp(tok, "find")) {
				tok = strtok(NULL, "\n ");

				if (tok || check_str_num(tok)) {
					int index = atoi(tok);

					find(arr, len, index);
				}
			} else if (!strcmp(tok, "delete_at")) {
				tok = strtok(NULL, "\n ");

				if (tok || check_str_num(tok)) {
					int index = atoi(tok);

					delete_at(&arr, &len, index);
				}
			} else if (!strcmp(tok, "print")) {
				print_all(arr, len);
			} else if (!strcmp(tok, "exit")) {
				// Frees the memory allocated for arr
				// Breaks the loop
				free(arr);
				break;
			}
		}
	}

	printf("\n");

	// Frees the memory allocated for "line"
	free(line);

	return 0;
}
