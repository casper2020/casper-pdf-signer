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
#ifndef CASPER_PDF_QPDF_READER_H_
#define CASPER_PDF_QPDF_READER_H_

#include "casper/pdf/reader.h"

#include "casper/pdf/annotation.h"

#include <inttypes.h>
#include <string>

#include "casper/pdf/qpdf/includes.h"

namespace casper
{

    namespace pdf
    {

        namespace qpdf
        {

            class Reader final : public ::casper::pdf::Reader
            {

            private: // Static Const Data
                
                static const char* const sk_byte_range_err_msg_prefix_;

            public: // Constructor(s) / Destructor
                
                Reader ();
                virtual ~Reader ();
                
            private: // Data(s)
                
                QPDF* pdf_;

            public: // Inherited Method(s) / Function(s) from pdf::Reader
                
                virtual void   Open         (const std::string& a_uri);
                virtual bool   GetByteRange (const ssize_t a_page, pdf::SignatureAnnotation& a_annotation);
                virtual size_t PageCount    ();
                virtual void   Close        ();
                                
            private: // Method(s) / Function(s)
                
                bool GetByteRange (QPDFAcroFormDocumentHelper& a_form,
                                   const std::vector<QPDFAnnotationObjectHelper>& a_annotations, const std::string& a_name, pdf::SignatureAnnotation::ByteRange& o_range) const;

            }; // end of class 'Reader'
    
        } // end of namespace 'apdf'
    
    } // end of namespace 'pdf'

} // end of namespace 'casper'
      
#endif // CASPER_PDF_QPDF_READER_H_
