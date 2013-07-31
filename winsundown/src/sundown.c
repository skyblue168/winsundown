/*
 * Copyright (c) 2011, Vicent Marti
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "markdown.h"
#include "html.h"
#include "buffer.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ_UNIT 1024
#define OUTPUT_UNIT 64

/* main • main function, interfacing STDIO with the parser */
int
main(int argc, char **argv)
{
	struct buf *ib, *ob;
	int ret;
	FILE *in = stdin;
	unsigned int extensions;
	char hHead[] = "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=""utf-8"">\n<link rel=stylesheet type=""text/css"" href=""ex.css"">\n</head>\n<body>\n";
	char hTail[] = "</body>\n</html>";

	struct sd_callbacks callbacks;
	struct html_renderopt options;
	struct sd_markdown *markdown;

	/* opening the file if given from the command line */
	if (argc > 1) {
		in = fopen(argv[1], "r");
		if (!in) {
			fprintf(stderr,"Unable to open input file \"%s\": %s\n", argv[1], strerror(errno));
			return 1;
		}
	}

	/* reading everything */
	ib = bufnew(READ_UNIT);
	bufgrow(ib, READ_UNIT);
	while ((ret = fread(ib->data + ib->size, 1, ib->asize - ib->size, in)) > 0) {
		ib->size += ret;
		bufgrow(ib, ib->size + READ_UNIT);
	}

	if (in != stdin)
		fclose(in);

	/* performing markdown parsing */
	ob = bufnew(OUTPUT_UNIT);

	//extensions = MKDEXT_TABLES|MKDEXT_FENCED_CODE|MKDEXT_AUTOLINK|MKDEXT_STRIKETHROUGH|MKDEXT_SUPERSCRIPT|MKDEXT_LAX_SPACING;
	extensions = 0xff;

	sdhtml_renderer(&callbacks, &options, 0);
	markdown = sd_markdown_new(extensions, 16, &callbacks, &options);

	sd_markdown_render(ob, ib->data, ib->size, markdown);
	sd_markdown_free(markdown);

	fwrite(hHead, 1, strlen(hHead), stdout);
	/* writing the result to stdout */
	ret = fwrite(ob->data, 1, ob->size, stdout);
	fwrite(hTail, 1, strlen(hTail), stdout);
	

	/* cleanup */
	bufrelease(ib);
	bufrelease(ob);

	return (ret < 0) ? -1 : 0;
}

/* vim: set filetype=c: */
