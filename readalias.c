#import <CoreFoundation/CoreFoundation.h>
#import <CoreServices/CoreServices.h>

int main(int argc, char *argv[]) {
	if (argc > 1) {
		BOOL skipN = 0;
		BOOL recurse = 0;

		int c = 0;
		while ((c = getopt(argc, argv, "fn")) != EOF) {
			switch (c) {
				case 'f':
					recurse ^= 1;
					break;
				case 'n':
					skipN ^= 1;
					break;
				default:
					goto usage;
			}
		}

		int numPrinted = 0;
		c = optind;
		while (c < argc) {
			CFURLRef url = CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8 *)argv[c], strlen(argv[c]), false);
			if (url != NULL) {
				FSRef ref;
				Boolean wasAlias = 0, isDir = 0;
				if (CFURLGetFSRef(url, &ref) && (FSResolveAliasFile(&ref, recurse, &isDir, &wasAlias) == 0)  && wasAlias) {
					CFURLRef resolved = CFURLCreateFromFSRef(NULL, &ref);
					if (resolved != NULL) {
						CFStringRef pathString = CFURLCopyFileSystemPath(resolved, kCFURLPOSIXPathStyle);
						if (pathString != NULL) {
							CFIndex maxlen = CFStringGetMaximumSizeOfFileSystemRepresentation(pathString);
							char *path = malloc(maxlen);
							if (path == NULL)
								err(1, NULL);
							CFStringGetFileSystemRepresentation(pathString, path, maxlen);
							fputs(path, stdout);
							numPrinted += 1;
							free(path);
							if (skipN == 0)
								putchar('\n');
							CFRelease(pathString);
						}
						CFRelease(resolved);
					}
				}
				CFRelease(url);
			}
			c += 1;
		}
		return !(numPrinted == (argc - optind));
	} else {
usage:
		fprintf(stderr, "usage:  %s [-fn] <path>...\n", argv[0]);
		return 1;
	}
}
