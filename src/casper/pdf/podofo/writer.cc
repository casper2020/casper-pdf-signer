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

#include "casper/pdf/podofo/writer.h"

#include "cc/exception.h"

#include "cc/types.h" // SIZET_FMT
#include "cc/fs/file.h"

#include "casper/pdf/podofo/annotation.h"

// MARK: -

const ::PoDoFo::PdfName casper::pdf::podofo::Writer::sk_fields_key_    = ::PoDoFo::PdfName("Fields");
const ::PoDoFo::PdfName casper::pdf::podofo::Writer::sk_parent_key_    = ::PoDoFo::PdfName("Parent");
const ::PoDoFo::PdfName casper::pdf::podofo::Writer::sk_sig_key_       = ::PoDoFo::PdfName("Sig");
const ::PoDoFo::PdfName casper::pdf::podofo::Writer::sk_byte_rage_key_ = ::PoDoFo::PdfName("ByteRange");
const ::PoDoFo::PdfName casper::pdf::podofo::Writer::sk_t_key_         = ::PoDoFo::PdfName("T");
const ::PoDoFo::PdfName casper::pdf::podofo::Writer::sk_ft_key_        = ::PoDoFo::PdfName("FT");
const ::PoDoFo::PdfName casper::pdf::podofo::Writer::sk_p_key_         = ::PoDoFo::PdfName("P");

// MARK: -

/**
 * @brief Default constructor.
 *
 * @param a_name Writer name.
 */
casper::pdf::podofo::Writer::Writer (const std::string& a_name)
 : casper::pdf::Writer(a_name)
{
    document_handler_ = nullptr;
    output_handler_   = nullptr;
    sign_handler_     = nullptr;
}

/**
 * @brief Destructor.
 */
casper::pdf::podofo::Writer::~Writer ()
{
    if ( nullptr != document_handler_ ) {
        delete document_handler_;
    }
    if ( nullptr != output_handler_ ) {
        delete output_handler_;
    }
    if ( nullptr != sign_handler_ ) {
        delete sign_handler_;
    }
}

// MARK: -

/**
 * @brief Open a PDF document and write any change to output.
 *
 * @param a_io Local file URI to read and to write to.
 */
void casper::pdf::podofo::Writer::Open (const std::string& a_io)
{
    Open(a_io, a_io);
}

/**
 * @brief Open a PDF document and write any change to output.
 *
 * @param a_in        Local file URI to read from.
 * @param a_out       Local file URI to write to.
 * @param a_overwrite When true, overwrite destination file ( if any ).
 */
void casper::pdf::podofo::Writer::Open (const std::string& a_in, const std::string& a_out, const bool a_overwrite)
{
    // ... already open?
    if ( nullptr != document_handler_ || nullptr != output_handler_ || nullptr != sign_handler_ ) {
        throw ::cc::Exception("Document is already open!");
    }
    // ... prepare ...
    try {
        document_handler_ = new ::PoDoFo::PdfMemDocument();
        document_handler_->Load(a_in.c_str(), /* bForUpdate */ true);
        // ... an existing copy is required ...
        if ( a_out != a_in ) {
            if ( false == cc::fs::File::Exists(a_out) ) {
                cc::fs::File::Copy(a_in, a_out);
            } else {
                cc::fs::File::Copy(a_in, a_out, 0 == cc::fs::File::Size(a_out));
            }
        }
        output_handler_ = new ::PoDoFo::PdfOutputDevice(a_out.c_str(), /* bTruncate */ false);
        sign_handler_   = new ::PoDoFo::PdfSignOutputDevice(output_handler_);
    } catch (const ::PoDoFo::PdfError& a_error) {
        Close();
        throw ::cc::Exception("PoDoFo Error: %4d - %s", a_error.GetError(), ::PoDoFo::PdfError::ErrorMessage(a_error.GetError()));
    } catch (...) {
        Close();
        cc::Exception::Rethrow(/* a_unhandled */ false, __FILE__, __LINE__, __FUNCTION__);
    }
}

/**
 * @brief Append a signature placeholder.
 *
 * @param a_annotation Annotation properties.
 */
void casper::pdf::podofo::Writer::Append (const pdf::SignatureAnnotation& a_annotation)
{
    // ... document must be open ..
    if ( nullptr == document_handler_ ) {
        throw ::cc::Exception("Document is not open!");
    }
    
    // ... document must have at least one page ...
    if ( 0 == document_handler_->GetPageCount() ) {
        throw ::cc::Exception("Document has no pages!");
    }
    
    ::PoDoFo::PdfSignatureField* signature_field = nullptr;
    
    try {
        
        // ... grab local ref to document ...
        ::PoDoFo::PdfMemDocument& document = *document_handler_;
        
        // ... grab 'form' ...
        ::PoDoFo::PdfAcroForm* acro_form = document.GetAcroForm();
        if ( nullptr == acro_form ) {
            throw ::cc::Exception("Can't find AcroFrom!");
        }
        
        const ::PoDoFo::PdfString name(a_annotation.name_);
        
        // ... update SigFlags ...
        {
            ::PoDoFo::PdfDictionary&  dictionary  = acro_form->GetObject()->GetDictionary();
            const ::PoDoFo::PdfName   flags_name  = ::PoDoFo::PdfName("SigFlags");
            const ::PoDoFo::pdf_int64 flags_value = 3;
            if ( false == dictionary.HasKey(flags_name) ) {
                dictionary.AddKey(flags_name, ::PoDoFo::PdfObject(flags_value));
            } else {
                const ::PoDoFo::PdfObject* actual = dictionary.GetKey(flags_name);
                if ( false == actual->IsNumber() || actual->GetNumber() != flags_value ) {
                    dictionary.RemoveKey(flags_name);
                    dictionary.AddKey(flags_name, ::PoDoFo::PdfObject(flags_value));
                }
            }
        }

        // TODO: CHECK THIS
        if ( acro_form->GetNeedAppearances() ) {
            acro_form->SetNeedAppearances(false);
        }
                
        ::PoDoFo::PdfPage* page = nullptr;

        // ... a signature with the same name present? ...
        if ( true == SignatureObjectExists(acro_form, name) ) {
            throw ::cc::Exception("A signature with the same name '%s' is already present - not replacing it!", name.GetString());
        }
        
        // ... search for specific page ...
        page = document.GetPage(static_cast<int>(a_annotation.page() - 1));
        if ( nullptr == page ) {
            throw ::cc::Exception("Page number " SIZET_FMT " not found!", a_annotation.page());
        }
        
        // ... add an annotation ...
        const ::PoDoFo::PdfRect rect = ::PoDoFo::PdfRect(
                a_annotation.rect().x_, page->GetPageSize().GetHeight() - a_annotation.rect().y_ - a_annotation.rect().h_,
                a_annotation.rect().w_, a_annotation.rect().h_
        );
            
        ::PoDoFo::PdfAnnotation* annotation = page->CreateAnnotation(::PoDoFo::ePdfAnnotation_Widget, rect);
        if ( true == a_annotation.visible() ) {
            annotation->SetFlags(::PoDoFo::ePdfAnnotationFlags_Print | ::PoDoFo::ePdfAnnotationFlags_Locked);
        } else {
            annotation->SetFlags(::PoDoFo::ePdfAnnotationFlags_Invisible |::PoDoFo::ePdfAnnotationFlags_Hidden | ::PoDoFo::ePdfAnnotationFlags_Locked);
        }
        
        signature_field = new ::PoDoFo::PdfSignatureField(annotation, acro_form, &document);
        
        casper::pdf::podofo::SignatureAnnotation sign_annotation(a_annotation);
        // ... prevent some apps from displaying annotation even it if it's not visible ...
        if ( true == a_annotation.visible() ) {
            sign_annotation.Draw(*annotation, rect, document, *signature_field);
        }
        
        signature_field->SetReadOnly(true);
        
        sign_handler_->SetSignatureSize(a_annotation.info().size_in_bytes_);
        if ( 0 == a_annotation.info().size_in_bytes_ ) {
            throw ::cc::Exception("Invalid signature size of " SIZET_FMT " !", a_annotation.info().size_in_bytes_);
        }
        
        signature_field->SetFieldName(name);
        signature_field->SetSignatureReason(::PoDoFo::PdfString(a_annotation.info().reason_));
        signature_field->SetSignatureDate(::PoDoFo::PdfDate()); // TODO: date by param?
        signature_field->SetSignature(*sign_handler_->GetSignatureBeacon());
        signature_field->SetSignatureCreator(::PoDoFo::PdfName(name_));

        // ... write new objects ...
        document.SetWriteMode(::PoDoFo::EPdfWriteMode::ePdfWriteMode_Compact);
        document.WriteUpdate(sign_handler_, /* bTruncate */ false);
        
        // ... ensure signature is in place ...
        if ( false == sign_handler_->HasSignaturePosition() ) {
            throw ::cc::Exception("Cannot find signature position in the document!");
        }
        
        // ... adjust 'ByteRange' for signature ...
        sign_handler_->AdjustByteRange();

        // ... write new contents ...
        sign_handler_->Flush();
        
        delete signature_field;
        
    } catch (const ::cc::Exception& a_cc_exception) {
        if ( nullptr != signature_field ) {
            delete signature_field;
        }
        throw a_cc_exception;
    } catch (const ::PoDoFo::PdfError& a_error) {
        if ( nullptr != signature_field ) {
            delete signature_field;
        }
        throw ::cc::Exception("PoDoFo Error: %4d - %s", a_error.GetError(), ::PoDoFo::PdfError::ErrorMessage(a_error.GetError()));
    }
}

/**
 * @brief Close the currenly open file.
 */
void casper::pdf::podofo::Writer::Close ()
{
    if ( nullptr != document_handler_ ) {
        delete document_handler_;
        document_handler_ = nullptr;
    }
    if ( nullptr != output_handler_ ) {
        delete output_handler_;
        output_handler_ = nullptr;
    }
    if ( nullptr != sign_handler_ ) {
        delete sign_handler_;
        sign_handler_ = nullptr;
    }
}


// MARK: -

void casper::pdf::podofo::Writer::GetByteRange (const std::string& a_in, pdf::SignatureAnnotation& a_annotation)
{
    ::PoDoFo::PdfSignatureField* disposable_signature_field = nullptr;
    ::PoDoFo::PdfAnnotation*     disposable_annotation      = nullptr;
    
    try {
        
        Open(a_in, a_in);
        
        // ... grab 'form' ...
        ::PoDoFo::PdfAcroForm* acro_form = document_handler_->GetAcroForm();
        if ( nullptr == acro_form ) {
            throw ::cc::Exception("Can't find AcroFrom!");
        }
            
        // ... get signature page ...
        const ::PoDoFo::PdfObject* sig_object = GetExistingSignatureObject(acro_form, a_annotation.name_);
        if ( nullptr == sig_object ) {
            throw ::cc::Exception("%s", "Signature not found!");
        }
        
        // ... get signature page ...
        const ::PoDoFo::PdfPage* ref_page = GetExistingSignaturePage(acro_form, a_annotation.name_);
        if ( nullptr == ref_page ) {
            throw ::cc::Exception("%s", "Signature reference page not found!");
        }
        
        ::PoDoFo::PdfPage* sig_page = const_cast<::PoDoFo::PdfPage*>(ref_page);
        
        disposable_annotation      = new ::PoDoFo::PdfAnnotation(const_cast<::PoDoFo::PdfObject*>(sig_object), sig_page);
        disposable_signature_field = new ::PoDoFo::PdfSignatureField(disposable_annotation);
                
        const ::PoDoFo::PdfArray& array = GetByteRangeArray(disposable_signature_field);
        
        a_annotation.Set(pdf::SignatureAnnotation::ByteRange({
            // ... bytes before '/Contents'
            static_cast<size_t>(array[0].GetNumber()), static_cast<size_t>(array[1].GetNumber()),
            // ... bytes after '/Contents'
            static_cast<size_t>(array[2].GetNumber()) , static_cast<size_t>(array[3].GetNumber())
        }));
        
        Close();
        
        delete disposable_signature_field;
        disposable_signature_field = nullptr;
        
        delete disposable_annotation;
        disposable_annotation = nullptr;
        
    } catch (const ::PoDoFo::PdfError& a_error) {
        throw ::cc::Exception("PoDoFo Error: %4d - %s", a_error.GetError(), ::PoDoFo::PdfError::ErrorMessage(a_error.GetError()));
    } catch (const ::cc::Exception& a_cc_exception) {
        if ( nullptr != disposable_signature_field ) {
            delete disposable_signature_field;
        }
        if ( nullptr != disposable_annotation ) {
            delete disposable_annotation;
        }
        Close();
        throw a_cc_exception;
    }
}

// MARK: - [PRIVATE]

/**
 * @brief Search for 'signature field' object.
 *
 * @param a_form Acro form object.
 * @param a_name Field name.
 * @param a_type Field type.
 */
::PoDoFo::PdfObject* casper::pdf::podofo::Writer::GetFieldObject (::PoDoFo::PdfAcroForm* a_form, const ::PoDoFo::PdfString& a_name, const ::PoDoFo::PdfName& a_type) const
{
    // ... search for specifc signature field by name ...
    const ::PoDoFo::PdfDictionary& dictionary = a_form->GetObject()->GetDictionary();
    const ::PoDoFo::PdfObject*     fields     = dictionary.GetKey(sk_fields_key_);
    const ::PoDoFo::PdfVecObjects* objects    = a_form->GetDocument()->GetObjects();
    
    // ... it' a reference?
    if ( ::PoDoFo::ePdfDataType_Reference == fields->GetDataType() ) {
        // ... fetch actual object ...
        fields = objects->GetObject(fields->GetReference());
    }
    
    // ... not found or not an array?
    if ( nullptr == fields || ::PoDoFo::ePdfDataType_Array != fields->GetDataType() ) {
        // ... done ...
        return nullptr;
    }
        
    const ::PoDoFo::PdfArray& rArray = fields->GetArray();
    for ( auto idx = 0 ; idx < rArray.size() ; ++idx) {
        
        const auto it = rArray[idx];
        // ... we're searchinf for a referece ...
        if ( ::PoDoFo::ePdfDataType_Reference != it.GetDataType() ) {
            // ... not it, next ...
            continue;
        }
                
        // ... ensure 'T' item exists ...
        ::PoDoFo::PdfObject *item = objects->GetObject(it.GetReference());
        if ( nullptr == item || false == item->GetDictionary().HasKey(sk_t_key_) || a_name != item->GetDictionary().GetKey(sk_t_key_)->GetString() ) {
            // ... not found, next ...
            continue;
        }
        
        // ... a field with the searching name was found, search for 'FT' item ...
        const ::PoDoFo::PdfObject *pFT = item->GetDictionary().GetKey(sk_ft_key_);
        if ( nullptr == pFT && true == item->GetDictionary().HasKey(sk_parent_key_) ) {
            const ::PoDoFo::PdfObject *pTemp = item->GetIndirectKey(sk_parent_key_);
            if ( nullptr == pTemp ) {
                throw ::cc::Exception("Invalid data type found while searching form FT item!");
            }
            pFT = pTemp->GetDictionary().GetKey(sk_ft_key_);
        }
        
        // ...  'FP ' MUST exist ...
        if ( nullptr == pFT ) {
            throw ::cc::Exception("FT item not found!");
        }
        
        // ... a field with the provided name was found ...
        const ::PoDoFo::PdfName fieldType = pFT->GetName();
        if ( fieldType == a_type ) {
            // ... and it's a signature field ..
            return item;
        }

    }

    // ... not found ...
    return nullptr;
}

/**
 * @brief Search for 'signature' object.
 *
 * @param a_form Acro form object.
 * @param a_name Signature name.
 *
 * @return True if exists, false otherwise.
 */
bool casper::pdf::podofo::Writer::SignatureObjectExists (::PoDoFo::PdfAcroForm* a_form, const ::PoDoFo::PdfString& a_name) const
{
    return ( nullptr != GetFieldObject(a_form, a_name, sk_sig_key_) );
}

/**
 * @brief Search for 'signature' object.
 *
 * @param a_form Acro form object.
 * @param a_name Signature name.
 *
 * @return Signature object.
 */
const ::PoDoFo::PdfObject* casper::pdf::podofo::Writer::GetExistingSignatureObject (::PoDoFo::PdfAcroForm* a_form, const ::PoDoFo::PdfString& a_name) const
{
    const ::PoDoFo::PdfObject* existing_sig_object = GetFieldObject(a_form, a_name, sk_sig_key_);
    if ( nullptr == existing_sig_object ) {
        throw ::cc::Exception("Can't find signature object!");
    }
    return existing_sig_object;
}

/**
 * @brief Search for 'page' object where the signature with a specifc name is located at.
 *
 * @param a_form Acro form object.
 * @param a_name Signature name.
 *
 * @return Page object.
 */
const ::PoDoFo::PdfPage* casper::pdf::podofo::Writer::GetExistingSignaturePage (::PoDoFo::PdfAcroForm* a_form, const ::PoDoFo::PdfString& a_name) const
{
    const ::PoDoFo::PdfObject* object = GetExistingSignatureObject(a_form, a_name);
    if ( false == object->GetDictionary().HasKey(sk_p_key_) ) {
        throw ::cc::Exception("Signature object named '%s' doesn't have a page reference!",
                              a_name.GetString()
        );
    }
    return document_handler_->GetPagesTree()->GetPage(object->GetDictionary().GetKey(sk_p_key_)->GetReference());
}

/**
 * @brief Search for '/ByteRange' object at a spefic signature field object..
 *
 * @param a_field Signature field object.
 *
 * @return /ByteRange array object.
 */
const ::PoDoFo::PdfArray& casper::pdf::podofo::Writer::GetByteRangeArray (const ::PoDoFo::PdfSignatureField* a_field) const
{
    const ::PoDoFo::PdfObject* signature_object = a_field->GetSignatureObject();
    if ( false == signature_object->GetDictionary().HasKey("ByteRange") ) {
        throw ::cc::Exception("Missing or Invalid /ByteRange!");
    }
    const ::PoDoFo::PdfObject* byte_range_object = signature_object->GetDictionary().GetKey("ByteRange");
    if ( nullptr == byte_range_object ) {
        throw ::cc::Exception("Missing or Invalid /ByteRange!");

    }
    return byte_range_object->GetArray();
}
