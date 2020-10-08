/**
 * @file annotation.h
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
#ifndef CASPER_PDF_PODOFO_ANNOTATION_H_
#define CASPER_PDF_PODOFO_ANNOTATION_H_

#include "cc/non-copyable.h"
#include "cc/non-movable.h"

#include <string>

#include "casper/pdf/annotation.h"

#include <podofo/podofo.h>

namespace casper
{

    namespace pdf
    {

        namespace podofo
        {
        
            class SignatureAnnotation final : public ::casper::pdf::SignatureAnnotation
            {
                
            public: // Constructor(s) / Destructor
                
                SignatureAnnotation () = delete;
                SignatureAnnotation (const std::string& a_name) = delete;
                SignatureAnnotation (const ::casper::pdf::SignatureAnnotation& a_annotation);
                
                virtual ~SignatureAnnotation ();
                
            public: // Method(s) / Function(s)
                
                void Draw (const ::PoDoFo::PdfAnnotation& a_annotation, const ::PoDoFo::PdfRect& a_rect,
                           ::PoDoFo::PdfDocument& a_document, ::PoDoFo::PdfSignatureField& a_field) const;
                
            }; // end of class 'SignatureAnnotation'
        
        } // end of namespace 'podofo'

    } // end of namespace 'pdf'

} // end of namespace 'casper'

#endif // CASPER_PDF_PODOFO_ANNOTATION_H_
