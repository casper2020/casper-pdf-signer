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
#ifndef CASPER_PDF_PODOFO_WRITER_H_
#define CASPER_PDF_PODOFO_WRITER_H_

#include <string>

#include "casper/pdf/writer.h"

#include <podofo/podofo.h>

namespace casper
{

    namespace pdf
    {

        namespace podofo
        {
        
            class Writer final : public pdf::Writer
            {
                
            private: // Static Const Data
                
                static const ::PoDoFo::PdfName sk_fields_key_;
                static const ::PoDoFo::PdfName sk_parent_key_;
                static const ::PoDoFo::PdfName sk_sig_key_;
                static const ::PoDoFo::PdfName sk_byte_rage_key_;
                static const ::PoDoFo::PdfName sk_t_key_;
                static const ::PoDoFo::PdfName sk_ft_key_;
                static const ::PoDoFo::PdfName sk_p_key_;
                
            private: // Data
                
                ::PoDoFo::PdfMemDocument*      document_handler_;
                ::PoDoFo::PdfOutputDevice*     output_handler_;
                ::PoDoFo::PdfSignOutputDevice* sign_handler_;
                
            public: // Constructor(s) / Destructor
                
                Writer () = delete;
                Writer (const std::string& a_name);
                
                virtual ~Writer ();
                
            public: // Inherited Method(s) / Function(s) from pdf::Writer
                
                virtual void Open   (const std::string& a_in, const std::string& a_out, const bool a_overwrite = false);
                virtual void Open   (const std::string& a_io);
                virtual void Append (const SignatureAnnotation& a_annotation);
                virtual void Close  ();
                
            private: // Helper(s)
                
                      ::PoDoFo::PdfObject* GetFieldObject             (::PoDoFo::PdfAcroForm* a_form, const ::PoDoFo::PdfString& a_name, const ::PoDoFo::PdfName& a_type) const;
                bool                       SignatureObjectExists      (::PoDoFo::PdfAcroForm* a_form, const ::PoDoFo::PdfString& a_name) const;
                const ::PoDoFo::PdfObject* GetExistingSignatureObject (::PoDoFo::PdfAcroForm* a_form, const ::PoDoFo::PdfString& a_name) const;
                const ::PoDoFo::PdfPage*   GetExistingSignaturePage   (::PoDoFo::PdfAcroForm* a_form, const ::PoDoFo::PdfString& a_name) const;
                const ::PoDoFo::PdfArray&  GetByteRangeArray          (const ::PoDoFo::PdfSignatureField* a_field) const;
                        
            public: // Method(s) / Function(s)
            
                void GetByteRange (const std::string& a_in, pdf::SignatureAnnotation& a_annotation);

            }; // end of class 'Annotation'
        
        } // end of namespace 'podofo'

    } // end of namespace 'pdf'

} // end of namespace 'casper'

#endif // CASPER_PDF_PODOFO_WRITER_H_
