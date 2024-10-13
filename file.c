#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define Position_forward(p) Position_move(p, 1)
#define Position_backward(p) Position_move(p, -1)

typedef struct File {
	char *name, *contents;
} File;

typedef struct Position {
	File *file;
	unsigned int index, line, col;
	unsigned char cur_char; 
} Position;

typedef struct PosRange {
	Position *a, *b;
} PosRange;

typedef enum ErrorKind {
	ERROR_SYNTAX,
	ERROR_RUNTIME
} ErrorKind;

typedef struct Error {
	ErrorKind kind;
	char *name, *message;
	PosRange *range;
} Error;

int Position_move(Position *position, signed char amount) {
	if (!position || !position->file || !position->file->contents) return 1;

	int new_index = position->index + amount;
	if (new_index < 0 || new_index >= (int)strlen(position->file->contents)) return EOF;

	position->index = new_index;
	position->cur_char = position->file->contents[new_index];

	position->line = 0;
	position->col = 0;

	for (unsigned int i = 0; i < position->index; i++) {
		if (position->file->contents[i] != '\n') {
			position->col++;
			continue;
		}

		position->line++;
		position->col = 0;
	}

	return 0;
}

Position *Position_clone(const Position *position) {
	if (!position) return NULL;

	Position *new_position = (Position *)malloc(sizeof(Position));
	if (!new_position) return NULL;

	new_position->file = position->file;
	new_position->index = position->index;
	new_position->line = position->line;
	new_position->col = position->col;
	new_position->cur_char = position->cur_char;

	return new_position;
}

Position *Position_from_file(File *file) {
	if (!file || !file->contents) return NULL;

	Position *position = (Position *)malloc(sizeof(Position));
	if (!position) return NULL;

	position->file = file;
	position->index = 0;
	position->line = 0;
	position->col = 0;
	position->cur_char = file->contents[0];

	return position;
}

char *Position_to_string(Position *position) {
	if (!position || !position->file || !position->file->name)
		return strdup("Invalid position structure");

	size_t buffer_size = strlen(position->file->name) + 20;
	char *pos_str = (char *)malloc(buffer_size);
	if (!pos_str) return NULL;

	sprintf(pos_str, "%s:%u:%u", position->file->name, position->line + 1, position->col + 1);
	return pos_str;
}

/** this does not free file */
void Position_free(Position *position) {
	free(position);
}

PosRange *PosRange_create(Position *a, Position *b) {
	if (!a || !b) return NULL;

	PosRange *range = (PosRange *)malloc(sizeof(PosRange));
	if (!range) return NULL;

	range->a = Position_clone(a);
	range->b = Position_clone(b);

	return range;
}

char *PosRange_to_string(PosRange *range) {
	if (!range || !range->a || !range->b || !range->a->file || !range->a->file->name) 
		return strdup("Invalid position range");

	char *pos_a_str = Position_to_string(range->a);
	if (!pos_a_str) return NULL;

	size_t buffer_size = strlen(pos_a_str) + 30;
	char *range_str = (char *)malloc(buffer_size);
	if (!range_str) {
		free(pos_a_str);
		return NULL;
	}

	sprintf(range_str, "%s-%u:%u", pos_a_str, range->b->line + 1, range->b->col + 1);
	free(pos_a_str);

	return range_str;
}

/** this frees the positions */
void PosRange_free(PosRange *range) {
	if (!range) return;
	Position_free(range->a);
	Position_free(range->b);
	free(range);
}

File *File_read(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (!file) return NULL;

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	rewind(file);

	char *buffer = (char *)malloc(file_size + 1);
	if (!buffer) {
		fclose(file);
		return NULL;
	}

	size_t read_size = fread(buffer, 1, file_size, file);
	if (read_size != (size_t)file_size) {
		free(buffer);
		fclose(file);
		return NULL;
	}

	buffer[file_size] = '\0';

	fclose(file);

	File *file_struct = (File *)malloc(sizeof(File));
	if (!file_struct) {
		free(buffer);
		return NULL;
	}

	file_struct->name = strdup(filename);
	file_struct->contents = buffer;

	return file_struct;
}

void File_free(File *file) {
	if (!file) return;

	free(file->name);
	free(file->contents);
	free(file);
}

Error *Error_create(ErrorKind kind, char *name, char *message, PosRange *range) {
	if (!name || !message || !range) return NULL;

	Error *error = (Error *)malloc(sizeof(Error));
	if (!error) return NULL;

	error->kind = kind;

	error->name = strdup(name);
	if (!error->name) {
		free(error);
		return NULL;
	}

	error->message = strdup(message);
	if (!error->message) {
		free(error->name);
		free(error);
		return NULL;
	}

	error->range = PosRange_create(range->a, range->b);
	if (!error->range) {
		free(error->name);
		free(error->message);
		free(error);
		return NULL;
	}

	return error;
}

char *Error_to_string(Error *error) {
	if (!error || !error->range) return strdup("Invalid error structure");

	char *range_str = PosRange_to_string(error->range);
	if (!range_str) return NULL;

	size_t buffer_size = strlen(range_str) + strlen(error->name) + strlen(error->message) + 5;
	char *error_str = (char *)malloc(buffer_size);
	if (!error_str) {
		free(range_str);
		return NULL;
	}

	sprintf(error_str, "%s %s: %s", range_str, error->name, error->message);
	free(range_str);

	return error_str;
}

/** this also frees position range */
void Error_free(Error *error) {
	if (!error) return;
	PosRange_free(error->range); 
	free(error->message);
	free(error);
}
