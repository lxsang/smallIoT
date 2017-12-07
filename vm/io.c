#include "io.h"

/* report a fatal system error */
noreturn sysError(char *s1, char *s2)
{
	ignore fprintf(stderr, "%s\n%s\n", s1, s2);
	ignore abort();
}

/* report a nonfatal system error */
noreturn sysWarn(char *s1, char *s2)
{
	ignore fprintf(stderr, "%s\n%s\n", s1, s2);
}

static int fr(FILE * fp, char *p, int s)
{
	int r;

	r = fread(p, s, 1, fp);
	if (r && (r != 1))
		sysError("imageRead count error", "");
	return r;
}

noreturn imageRead(FILE * fp)
{
	short i, size;
	struct __dummy_obj__ dummyObject;
	ignore fr(fp, (char *) &symbols, sizeof(object));
	i = 0;
	while (fr(fp, (char *) &dummyObject, sizeof(dummyObject))) {
		i = dummyObject.di;
		if ((i < 0) || (i > ObjectTableMax))
			sysError("reading index out of range", "");
		objectTable[i].class = dummyObject.cl;
		if ((objectTable[i].class < 0) ||
		((objectTable[i].class >> 1) > ObjectTableMax)) {
			fprintf(stderr, "index %d\n", dummyObject.cl);
			sysError("class out of range", "imageRead");
		}
		objectTable[i].size = size = dummyObject.ds;
		if (size < 0)
			size = ((-size) + 3) / 4;
		if (size != 0) {
			objectTable[i].memory = mBlockAlloc((int) size);
			ignore fr(fp, (char *) objectTable[i].memory,
			sizeof(object) * (int) size);
		} else
			objectTable[i].memory = (object *) 0;
	}
	/* now restore ref counts, getting rid of unneeded junk */
	visit(symbols);
	/* toss out the old free lists, build new ones */
	setFreeLists();

}