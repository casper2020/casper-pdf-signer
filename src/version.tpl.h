/**
 * @file version.h
 *
 * Copyright (c) 2011-2020 Cloudware S.A. All rights reserved.
 *
 * This file is part of casper-pdf-signer.
 *
 * casper-pdf-signer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * casper-pdf-signer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with casper-pdf-signer. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#ifndef CASPER_PDF_SIGNER_VERSION_H_
#define CASPER_PDF_SIGNER_VERSION_H_

#ifndef CASPER_PDF_SIGNER_ABBR
#define CASPER_PDF_SIGNER_ABBR "cps"
#endif

#ifndef CASPER_PDF_SIGNER_NAME
#define CASPER_PDF_SIGNER_NAME "casper-pdf-signer@b.n.s@"
#endif

#ifndef CASPER_PDF_SIGNER_VERSION
#define CASPER_PDF_SIGNER_VERSION "x.x.x"
#endif

#ifndef CASPER_PDF_SIGNER_REL_DATE
#define CASPER_PDF_SIGNER_REL_DATE "r.r.d"
#endif

#ifndef CASPER_PDF_SIGNER_REL_BRANCH
#define CASPER_PDF_SIGNER_REL_BRANCH "r.r.b"
#endif

#ifndef CASPER_PDF_SIGNER_REL_HASH
#define CASPER_PDF_SIGNER_REL_HASH "r.r.h"
#endif

#ifndef CASPER_PDF_SIGNER_INFO
#define CASPER_PDF_SIGNER_INFO CASPER_PDF_SIGNER_NAME " v" CASPER_PDF_SIGNER_VERSION
#endif

#define CASPER_PDF_SIGNER_BANNER \
"  ____    _    ____  ____  _____ ____            ____  ____  _____           ____ ___ ____ _   _ _____ ____  " \
"\n / ___|  / \\  / ___||  _ \\| ____|  _ \\          |  _ \\|  _ \\|  ___|         / ___|_ _/ ___| \\ | | ____|  _ \\ " \
"\n| |     / _ \\ \\___ \\| |_) |  _| | |_) |  _____  | |_) | | | | |_     _____  \\___ \\| | |  _|  \\| |  _| | |_) |" \
"\n| |___ / ___ \\ ___) |  __/| |___|  _ <  |_____| |  __/| |_| |  _|   |_____|  ___) | | |_| | |\\  | |___|  _ < " \
"\n \\____/_/   \\_\\____/|_|   |_____|_| \\_\\         |_|   |____/|_|             |____/___\\____|_| \\_|_____|_| \\_\\"

#endif // CASPER_PDF_SIGNER_VERSION_H_
