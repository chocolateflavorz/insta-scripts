#include "../../externallib/toml-c.h"
#include "../../headers/config.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int load()
{
    FILE* fp;
    char buf[4096];
    char errbuf[200];

    // 1. Read and parse toml file
    fp = fopen("sample.toml", "r");
    if (!fp) {
        error("cannot open sample.toml - ", strerror(errno));
    }
    size_t read_bytes = fread(buf, 1, sizeof(buf), fp);
    if (read_bytes == sizeof(buf)) {
        error("config file too large");
    }
    fclose(fp);
	
	toml_table_t *tbl = toml_parse(doc, errbuf, sizeof(errbuf));
	if (!tbl) {
		fprintf(stderr, "ERROR: %s\n", errbuf);
		exit(1);
	}

	// Get specific keys.
	toml_value_t host = toml_table_string(tbl, "host");
	toml_value_t port = toml_table_int(tbl, "port");
	if (!host.ok) // Default values.
		host.u.s = "localhost";
	if (!port.ok)
		host.u.i = 80;
	printf("%s:%ld\n", host.u.s, port.u.i);

	// Get a table.
	toml_table_t *sub_tbl = toml_table_table(tbl, "tbl");
	if (sub_tbl) {
		// Loop over all keys in a table.
		int l = toml_table_len(sub_tbl);
		for (int i = 0; i < l; i++) {
			int keylen;
			const char *key = toml_table_key(sub_tbl, i, &keylen);
			printf("key #%d: %s\n", i, key);
			// TODO: this should return toml_key_t or something, which also
			// includes the type. This actually requires a bit of frobbing with
			// the lexer, as that just sets the type of everything to STRING.
			//
			// Then we can also get rid of toml_table_{string,int,...} and just
			// parse it automatically.
		}
	}
}