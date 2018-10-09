#include <lconvert.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc < 2)
		return -1;

	LConvert convert;

	FILE* fin = fopen(argv[1],"r");
	if (!fin)
		return -2;

	fseek(fin, 0, SEEK_END);
	long size = ftell(fin);
	char *buf = (char*)malloc(size+1);
	fseek(fin, 0, SEEK_SET);
	size_t read = fread(buf, 1, size, fin);
	if (read)
	{
		buf[read] = 0;
		std::string out;
		convert.Convert(buf, out);

		std::string outName = argv[1];
		outName.append(".txt");
		FILE* fout = fopen(outName.c_str(), "w");
		if (fout)
		{
			fwrite(out.c_str(), out.length(), 1, fout);
			fclose(fout);
		}
	}
	fclose(fin);
	return 0;
}


