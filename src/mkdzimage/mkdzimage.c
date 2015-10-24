/*
 * Copyright (c) 2012, 2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "dzimage.h"

extern char *optarg;
extern int optind, opterr, optopt;
static char optstr[] = "o:k:d:a:";

static void usage(char *name)
{
	fprintf(stderr, "usage: %s [-okda]\n", name);
	fprintf(stderr, "\t -o\toutput filename\n");
	fprintf(stderr, "\t -k\tkernel filename\n");
	fprintf(stderr, "\t -d\tdevice tree filename\n");
	fprintf(stderr, "\t -a\tatags address\n");
	exit(0);
}

static void *load_file(char *file, unsigned int *total)
{
	FILE *fp;
	void *data;
	int ret, size;

	fp = fopen(file, "rb");
	if (!fp)
		goto out;

	ret = fseek(fp, 0x0, SEEK_END);
	if (ret < 0)
		goto close;

	size = ftell(fp);
	if (size < 1)
		goto close;

	ret = fseek(fp, 0x0, SEEK_SET);
	if (ret < 0)
		goto out;

	data = malloc(size);
	if (!data)
		goto out;

	if (fread(data, sizeof(char), size, fp) != sizeof(char) *size)
		goto free_data;

	*total = size;

	fclose(fp);

	return data;

free_data:
	free(data);
close:
	fclose(fp);
out:
	return NULL;
}

static unsigned char align_data[ALIGN_SIZE] = { 0, };
static void write_align(FILE *fp, unsigned int align_size, unsigned int cur_size)
{
	unsigned int write_size;

	if (!fp)
		return;

	cur_size = cur_size % align_size;
	write_size = align_size - cur_size;

	if (!cur_size || !write_size)
		return;

	fwrite(align_data, write_size, sizeof(unsigned char), fp);
}

int main(int argc, char **argv)
{
	struct dzimage_hdr hdr;
	char out_name[MAX_NAME+1];
	char kern_name[MAX_NAME+1];
	char dt_name[MAX_NAME+1];
	unsigned int kern_addr = 0x00008000;
	unsigned int atags_addr = 0x01e00000;
	void *kern_data, *dt_data;
	FILE *fp;
	int c;

	memset(&hdr, 0x0, sizeof(hdr));
	memset(out_name, 0x0, MAX_NAME+1);
	memset(kern_name, 0x0, MAX_NAME+1);
	memset(dt_name, 0x0, MAX_NAME+1);

	opterr = 0;
	while ((c = getopt(argc, argv, optstr)) != -1) {
		switch (c) {
		case 'o':
			strcpy(out_name, optarg);
			break;
		case 'k':
			strcpy(kern_name, optarg);
			break;
		case 'd':
			strcpy(dt_name, optarg);
			break;
		case 'a':
			sscanf(optarg, "%d", &atags_addr);
			break;
		default:
			usage(argv[0]);
			break;
		}
	}

	printf("out_name[%s]\n", out_name);
	printf("kern_name[%s]\n", kern_name);
	printf("dt_name[%s]\n", dt_name);

	hdr.magic_num = MAGIC_NUM;
	hdr.kernel_addr = kern_addr;
	hdr.tags_addr = atags_addr;
	hdr.page_size = ALIGN_SIZE;

	kern_data = load_file(kern_name, &hdr.kernel_size);
	if (!kern_data) {
		fprintf(stderr, "failed to load %s\n", kern_name);
		goto kern_load;
	}

	hdr.dt_addr = kern_addr + hdr.kernel_size;
	dt_data = load_file(dt_name, &hdr.dt_size);
	if (!dt_data) {
		fprintf(stderr, "failed to load %s\n", dt_name);
		goto dt_load;
	}

	fp = fopen(out_name, "wb");
	if (!fp) {
		fprintf(stderr, "failed to open %s\n", out_name);
		goto clear;
	}

	fwrite(&hdr, sizeof(hdr), sizeof(unsigned char), fp);
	write_align(fp, ALIGN_SIZE, sizeof(hdr));

	fwrite(kern_data, hdr.kernel_size, sizeof(unsigned char), fp);
	write_align(fp, ALIGN_SIZE, hdr.kernel_size);

	fwrite(dt_data, hdr.dt_size, sizeof(unsigned char), fp);
	write_align(fp, ALIGN_SIZE, hdr.dt_size);

	fclose(fp);

	free(kern_data);
	free(dt_data);

	return 0;

clear:
	free(dt_data);
dt_load:
	free(kern_data);
kern_load:
	exit(0);
	return 1;
}
