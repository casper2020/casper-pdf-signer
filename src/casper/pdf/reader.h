/**
 * @file reader.h
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
 * along with casper. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CASPER_PDF_READER_H_
#define CASPER_PDF_READER_H_

#include "cc/non-copyable.h"
#include "cc/non-movable.h"

#include <string>

#include "casper/pdf/annotation.h"

namespace casper
{

    namespace pdf
    {
    
        class Reader : public ::cc::NonCopyable, public ::cc::NonMovable
        {

        public: // Constructor(s) / Destructor
            
            Reader ();
            virtual ~Reader ();

        public: // Pure Virtual Method(s) / Function(s)
            
            virtual void   Open         (const std::string& a_uri) = 0;
            virtual bool   GetByteRange (const ssize_t a_page, pdf::SignatureAnnotation& o_annotation) = 0;
            virtual size_t PageCount    () = 0;
            virtual void   Close        () = 0;
            
        }; // end of class 'Reader'
    
    } // end of namespace 'pdf'

} // end of namespace 'casper'
      
#endif // CASPER_PDF_READER_H_
