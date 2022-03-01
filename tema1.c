#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define NMAX 10000
#define NBUFF 64

// Functia de conversie dintr-o baza in alta.
char *convertor(unsigned int numar, int baza)
{
	static char buff[NBUFF];
	char *iterator = &buff[NBUFF - 1];
	*iterator = '\0';
	static char *baza16 = "0123456789abcdef";

	while (numar != 0) {
		--iterator;
		*iterator = baza16[numar % baza];
		numar = numar / baza;
	}

	return iterator;
}

static int write_stdout(const char *token, int length)
{
	int rc;
	int bytes_written = 0;

	do {
		rc = write(1, token + bytes_written, length - bytes_written);
		if (rc < 0)
			return rc;

		bytes_written += rc;
	} while (bytes_written < length);

	return bytes_written;
}

int iocla_printf(const char *format, ...)
{
	char *buff = (char *)calloc(NMAX, sizeof(char));
	if (buff == NULL) {
		fprintf(stderr, "Nu s-a putut aloca memorie");
		return -1;
	}

	va_list args;
	va_start(args, format);

	int contor = 0;
	int nr_bytes = 0;

	for (int i = 0; i < (int)strlen(format); i++) {
		// Verific daca am dat peste un %, altfel adaug caracterul la buffer.
		if (format[i] != '%') {
			buff[contor] = format[i];
			contor++;
		}
		// Verific specificatorul de format si adaug caracterele in buffer.
		if (format[i] == '%') {
			i++;
			if (format[i] == 'c') {
				int char_container = va_arg(args, int);
				buff[contor] = (char)char_container;
				contor++;
			}

			if (format[i] == 's') {
				char *s_cont = va_arg(args, char *);
				strncat(buff, s_cont, strlen(s_cont));
				contor += (int)strlen(s_cont);
			}

			if (format[i] == 'x') {
				int int_aux = va_arg(args, unsigned int);
				char *s_cont = convertor(int_aux, 16);
				strncat(buff, s_cont, strlen(s_cont));
				contor += (int)strlen(s_cont);
			}

			if (format[i] == 'u') {
				unsigned int int_aux = va_arg(args, unsigned int);
				char *s_cont = convertor(int_aux, 10);
				strncat(buff, s_cont, strlen(s_cont));
				contor += (int)strlen(s_cont);
			}

			if (format[i] == 'd') {
				int int_aux = va_arg(args, int);
				if (int_aux < 0) { // Verific daca e nevoie sa adaug '-'.
					char *s_cont = convertor(-int_aux, 10);
					buff[contor] = '-';
					contor++;
					strncat(buff, s_cont, strlen(s_cont));
					contor += (int)strlen(s_cont);
				} else {
					char *s_cont = convertor(int_aux, 10);
					strncat(buff, s_cont, strlen(s_cont));
					contor += (int)strlen(s_cont);
				}
			}
			// Daca dau peste alt %, il escapez si il adaug in buffer.
			if (format[i] == '%') {
				buff[contor] = '%';
				contor++;
			}
		}
	}
	va_end(args);
	nr_bytes = strlen(buff); // Nr de bytes de afisat
	write_stdout(buff, strlen(buff)); // Afisare buffer la consola
	free(buff);

	if (nr_bytes != 0) {
		return nr_bytes;
	} else {
		return -1;
	}
}
