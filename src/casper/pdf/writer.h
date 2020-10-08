/**
 * @file writer.h
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
#ifndef CASPER_PDF_WRITER_H_
#define CASPER_PDF_WRITER_H_

#include "cc/non-copyable.h"
#include "cc/non-movable.h"

#include <string>

#include "casper/pdf/annotation.h"

namespace casper
{

    namespace pdf
    {
    
        class Writer : public ::cc::NonCopyable, public ::cc::NonMovable
        {

        public: // Const Data
            
            const std::string name_;            

        public: // Constructor(s) / Destructor
            
            Writer () = delete;
            Writer (const std::string& a_name);
            
            virtual ~Writer ();

        public: // Pure Virtual Method(s) / Function(s)
            
            virtual void Open   (const std::string& a_in, const std::string& a_out, const bool a_overwrite = false) = 0;
            virtual void Open   (const std::string& a_io) = 0;
            virtual void Append (const SignatureAnnotation& a_annotation) = 0;
            virtual void Close  () = 0;
            
        }; // end of class 'Writer'
    
    } // end of namespace 'pdf'

} // end of namespace 'casper'
      
#endif // CASPER_PDF_WRITER_H_
