#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#pragma warning(disable:4996)

typedef struct TGAHeader
{
	uint8_t idlength;
	uint8_t colourmaptype;
	uint8_t datatypecode;
	uint16_t colourmaporigin;
	uint16_t colourmaplength;
	uint8_t colourmapdepth;
	uint16_t x_origin;
	uint16_t y_origin;
	uint16_t width;
	uint16_t height;
	uint8_t bitsperpixel;
	uint8_t imagedescriptor;

} TGAHeader;

typedef struct RGBcolor
{
	uint8_t B;
	uint8_t G;
	uint8_t R;
} RGBcolor;

typedef struct Matrix
{
	int rows;
	int cols;

	int size;

	RGBcolor* data;

} Matrix;


void Matrix_init(Matrix* matrix, int rows, int cols)
{
	matrix->rows = rows;
	(*matrix).cols = cols;

	matrix->size = rows * cols;

	matrix->data = (RGBcolor*)malloc(sizeof(RGBcolor) * matrix->size);

	for (int i = 0; i < matrix->size; i++)
	{
		matrix->data[i].R = 0;
		matrix->data[i].G = 0;
		matrix->data[i].B = 0;
	}
}


void Matrix_set_value(Matrix* matrix, int row, int col, RGBcolor pixel)
{
	if (row < matrix->rows && col < matrix->cols && row >= 0 && col >= 0 && matrix->data != NULL) {
		matrix->data[row * matrix->cols + col] = pixel;
	}
}

void Matrix_destroy(Matrix** matrix)
{
	free((*matrix)->data);
	//(*matrix)->data = NULL;
	free(*matrix);
	*matrix = NULL;
}


void Matrix_load_from_file(Matrix** readMatrix, TGAHeader* header, const char* filename)
{
	FILE* fin = fopen(filename, "rb");

	if (fin != NULL)
	{
		fread(&header->idlength, sizeof(header->idlength), 1, fin);
		fread(&header->colourmaptype, sizeof(header->colourmaptype), 1, fin);
		fread(&header->datatypecode, sizeof(header->datatypecode), 1, fin);
		fread(&header->colourmaporigin, sizeof(header->colourmaporigin), 1, fin);
		fread(&header->colourmaplength, sizeof(header->colourmaplength), 1, fin);
		fread(&header->colourmapdepth, sizeof(header->colourmapdepth), 1, fin);
		fread(&header->x_origin, sizeof(header->x_origin), 1, fin);
		fread(&header->y_origin, sizeof(header->y_origin), 1, fin);
		fread(&header->width, sizeof(header->width), 1, fin);
		fread(&header->height, sizeof(header->height), 1, fin);
		fread(&header->bitsperpixel, sizeof(header->bitsperpixel), 1, fin);
		fread(&header->imagedescriptor, sizeof(header->imagedescriptor), 1, fin);

		*readMatrix = (Matrix*)malloc(sizeof(Matrix));
		Matrix_init(*readMatrix, header->height, header->width);

		fread((*readMatrix)->data, sizeof((*readMatrix)->data[0]), (*readMatrix)->size, fin);

		fclose(fin);
	}
	else
	{
		printf("Spatna cesta k souboru. \n");
	}
}

void save_tga(TGAHeader header, Matrix image, const char* filename) {
	FILE* tgaFile = fopen(filename, "wb");

	if (tgaFile != NULL) {
		fwrite(&header.idlength, sizeof(header.idlength), 1, tgaFile);
		fwrite(&header.colourmaptype, sizeof(header.colourmaptype), 1, tgaFile);
		fwrite(&header.datatypecode, sizeof(header.datatypecode), 1, tgaFile);
		fwrite(&header.colourmaporigin, sizeof(header.colourmaporigin), 1, tgaFile);
		fwrite(&header.colourmaplength, sizeof(header.colourmaplength), 1, tgaFile);
		fwrite(&header.colourmapdepth, sizeof(header.colourmapdepth), 1, tgaFile);
		fwrite(&header.x_origin, sizeof(header.x_origin), 1, tgaFile);
		fwrite(&header.y_origin, sizeof(header.y_origin), 1, tgaFile);
		fwrite(&header.width, sizeof(header.width), 1, tgaFile);
		fwrite(&header.height, sizeof(header.height), 1, tgaFile);
		fwrite(&header.bitsperpixel, sizeof(header.bitsperpixel), 1, tgaFile);
		fwrite(&header.imagedescriptor, sizeof(header.imagedescriptor), 1, tgaFile);

		for (int i = 0; i < image.size; i++) {
			fwrite(&image.data[i].B, sizeof(image.data[i].B), 1, tgaFile);
			fwrite(&image.data[i].G, sizeof(image.data[i].G), 1, tgaFile);
			fwrite(&image.data[i].R, sizeof(image.data[i].R), 1, tgaFile);
		}

		fclose(tgaFile);
	}
}

void image_draw_frame(Matrix image, RGBcolor pixel, int width) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < image.cols; j++) {
			image.data[i * image.cols + j] = pixel;
		}
	}

	for (int i = image.rows - 1; i >= image.rows - width; i--) {
		for (int j = 0; j < image.cols; j++) {
			image.data[i * image.cols + j] = pixel;
		}
	}

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < width; j++) {
			image.data[i * image.cols + j] = pixel;
		}
	}

	for (int i = 0; i < image.rows; i++) {
		for (int j = image.cols - 1; j >= image.cols - width; j--) {
			image.data[i * image.cols + j] = pixel;
		}
	}
}

void resizeImg(Matrix** obrazek, TGAHeader* header, int sirka, int vyska)
{
	if (sirka <= 0 || vyska <= 0)
	{
		printf("Neplatne argumenty\n");
		return;
	}
	Matrix* novyObrazek;

	novyObrazek = (Matrix*)malloc(sizeof(Matrix));
	Matrix_init(novyObrazek, vyska, sirka);

	float dy = (*obrazek)->cols / (float)sirka;
	float dx = (*obrazek)->rows / (float)vyska;

	int index = 0;
	for (int i = 0; i < vyska; i++)
	{
		for (int j = 0; j < sirka; j++)
		{
			index = (int)(dx * (float)i) * (*obrazek)->cols + (int)(dy * (float)j);
			novyObrazek->data[i * sirka + j] = (*obrazek)->data[index];
		}
	}

	header->height = vyska;
	header->width = sirka;
	Matrix_destroy(obrazek);
	*obrazek = novyObrazek;
}

void cropImage(Matrix** obrazek, TGAHeader* header, int sirka, int vyska, int x, int y)
{
	if (sirka <= 0 || vyska <= 0)
	{
		printf("Neplatne argumenty\n");
		return;
	}

	Matrix* novyObrazek;
	novyObrazek = (Matrix*)malloc(sizeof(Matrix));
	Matrix_init(novyObrazek, vyska, sirka);

	for (int i = x; i < x + vyska && i < (header)->height; i++)
	{
		for (int j = y; j < y + sirka && j < (header)->width; j++)
		{
			novyObrazek->data[(i - x) * sirka + (j - y)] = (*obrazek)->data[i * (*obrazek)->cols + j];
		}
	}
	header->height = vyska;
	header->width = sirka;
	Matrix_destroy(obrazek);
	*obrazek = novyObrazek;

	return;
}

int main(int argc, char* argv[])
{
	RGBcolor pixel;
	pixel.B = 255;
	pixel.G = 0;
	pixel.R = 0;
	
	TGAHeader header;
	Matrix* vstupni_obrazek;
	/*Matrix_load_from_file(&vstupni_obrazek, &header, "only10.tga");
	resizeImg(&vstupni_obrazek, &header, 2365, 140);
	save_tga(header, *vstupni_obrazek, "zzz.tga");
	*/
	if (argc >= 5)
	{
		if (strcmp(argv[1], "-i") == 0)
		{
			if (strcmp(argv[3], "-o") == 0)
			{
				Matrix_load_from_file(&vstupni_obrazek, &header, argv[2]);

				int i = 5;
				while (i < argc)
				{
					if (strcmp(argv[i], "-r") == 0)
					{
						printf("Operace resize \n");
						if (argc >= i + 3)
						{
							int w = atoi(argv[i + 1]);
							int h = atoi(argv[i + 2]);
							resizeImg(&vstupni_obrazek, &header, w, h); // sirka == cols, vyska == rows
						}
						else
						{
							break;
						}
						i += 3;
					}
					else if (strcmp(argv[i], "-c") == 0)
					{
						printf("Operace crop \n");
						if (argc >= i + 5)
						{
							int w = atoi(argv[i + 1]);
							int h = atoi(argv[i + 2]);
							int x = atoi(argv[i + 3]);
							int y = atoi(argv[i + 4]);
							cropImage(&vstupni_obrazek, &header, w, h, x, y); // sirka == cols, vyska == rows
						}
						i += 5;
					}
					else
					{
						printf("Neplatna operace \n");
						i++;
					}
				}

				save_tga(header, *vstupni_obrazek, argv[4]);
			}
			else
			{
				printf("Treti argument musi byt \"-o\".\n");
			}
		}
		else
		{
			printf("Prvni argument musi byt \"-i\".\n");
		}
	}
	else
	{
		printf("Nebyl zadan dostatecny pocet argumentu.\n");
	}

	return 0;
}

/*
	cesta s argumenty v konzoli: 

	C:\Users\Josef\source\repos\tga_manipulator\tga_manipulator>..\Debug\tga_manipulator.exe -i "only10.tga" -o "xxx.tga" -r 1000 1000
*/