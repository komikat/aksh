#ifndef HEADERS_H_
#define HEADERS_H_

// Standard C library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Additional system headers
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <termios.h>
#include <ctype.h>

// Custom headers
#include "proclore.h"
#include "prompt.h"
#include "warp.h"
#include "helpers.h"
#include "list.h"
#include "pastevents.h"
#include "peek.h"
#include "run.h"
#include "iman.h"
#include "run_expr.h"
#include "libproc.h"

#endif