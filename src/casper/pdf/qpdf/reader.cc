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

#include "casper/pdf/qpdf/reader.h"

#include "cc/exception.h"
#include "cc/types.h"

// MARK: -

const char* const casper::pdf::qpdf::Reader::sk_byte_range_err_msg_prefix_ = "Unable to obtain /Sig/ByteRange";

// MARK: -

/**
 * @brief Default constructor.
 */
casper::pdf::qpdf::Reader::Reader ()
{
    pdf_ = nullptr;
}

/**
 * @brief Destructor.
 */
casper::pdf::qpdf::Reader::~Reader ()
{
    if ( nullptr != pdf_ ) {
        delete pdf_;
    }
}

// MARK: -

/**
 * @brief Open a PDF document in read only mode..
 *
 * @param a_uri Local file URI to read and to write to.
 */
void casper::pdf::qpdf::Reader::Open (const std::string& a_uri)
{
    if ( nullptr != pdf_ ) {
        throw cc::Exception("%s", "Can't load '%s', already in use ( close it first! )");
    }
    pdf_ = new QPDF();
    pdf_->processFile(a_uri.c_str());
}

/**
 * @brief Read page count.
 *
 * @return Number of page of the currently loaded PDF document.
 */
size_t casper::pdf::qpdf::Reader::PageCount ()
{
    if ( nullptr == pdf_ ) {
        throw cc::Exception("%s", "Can't read page count - document is not open!");
    }

    QPDFObjectHandle root  = pdf_->getRoot();
    QPDFObjectHandle pages = root.getKey("/Pages");
    QPDFObjectHandle count = pages.getKey("/Count");
    
    if ( true == root.isNull() || true == pages.isNull() || true == count.isNull() ) {
        throw cc::Exception("%s", "Can't find /Pages/Count object!");
    }
    
    return static_cast<size_t>(count.getNumericValue());
}

/**
 * @brief Search for the signature annotation widget and read it's /ByteRange.
 *
 * @param a_page       Page number, if set it will search only in that page, -1 to search in all.
 * @param a_annotation Annotation to look for and where byte range will be set.
 *
 * @return True if found, false othewise.
 */
bool casper::pdf::qpdf::Reader::GetByteRange (const ssize_t a_page, pdf::SignatureAnnotation& a_annotation)
{
    if ( nullptr == pdf_ ) {
        throw cc::Exception("%s", "Can't read byte range - document is not open!");
    }
    
    QPDFAcroFormDocumentHelper afdh(*pdf_);
    QPDFPageDocumentHelper     pdh(*pdf_);
    
    pdf::SignatureAnnotation::ByteRange range;
    
    const std::vector<QPDFPageObjectHelper> pages = pdh.getAllPages();
    
    // ... a_page : 1..n ...
    if ( a_page < 0 ) { // ... backward lookup ...
        // ... searching in all pages ...
        for ( std::reverse_iterator<std::vector<QPDFPageObjectHelper>::const_iterator> page_iter = pages.rbegin(); page_iter != pages.rend(); ++page_iter ) {
            // ... search in all available annotations on the current page ...
            if ( true == GetByteRange(afdh, afdh.getWidgetAnnotationsForPage(*page_iter), a_annotation.name_, range) ) {
                // ... set range ...
                a_annotation.Set(range);
                // ... found ...
                return true;
            }
        }
    } else if ( 0 == a_page ) { // ... forward lookup ...
        // ... searching in all pages ...
        for ( std::vector<QPDFPageObjectHelper>::const_iterator page_iter = pages.begin(); page_iter != pages.end(); ++page_iter ) {
            // ... search in all available annotations on the current page ...
            if ( true == GetByteRange(afdh, afdh.getWidgetAnnotationsForPage(*page_iter), a_annotation.name_, range) ) {
                // ... set range ...
                a_annotation.Set(range);
                // ... found ...
                return true;
            }
        }
    } else if ( a_page <= pages.size() ) { // ... specifc page lookup ...
        const std::vector<QPDFPageObjectHelper>::const_iterator it = pages.begin() + static_cast<size_t>(a_page - 1);
        // ... search in all available annotations on the current page ...
        if ( true == GetByteRange(afdh, afdh.getWidgetAnnotationsForPage(*it), a_annotation.name_, range) ) {
            // ... set range ...
            a_annotation.Set(range);
            // ... found ...
            return true;
        }
    } else {
        throw cc::Exception("%s - page number " SIZET_FMT " not found!", sk_byte_range_err_msg_prefix_, a_page);
    }
    
    // ... not found ...
    return false;
}

/**
 * @brief Close the currenly open file.
 */
void casper::pdf::qpdf::Reader::Close ()
{
    if ( nullptr == pdf_ ) {
        return;
    }
    delete pdf_;
    pdf_ = nullptr;
}

// MARK: - [PRIVATE] - Helpers

/**
 * @brief Search for the signature annotation widget and read it's /ByteRange.
 *
 * @param a_form        Page number, if set it will search only in that page, -1 to search in all.
 * @param a_annotations Available annotations.
 * @param a_name        Annotation name to look for.
 * @param o_range       Signature byte range.
 *
 * @return True if found, false othewise.
 */
bool casper::pdf::qpdf::Reader::GetByteRange (QPDFAcroFormDocumentHelper& a_form,
                                              const std::vector<QPDFAnnotationObjectHelper>& a_annotations, const std::string& a_name,
                                              SignatureAnnotation::ByteRange& o_range) const
{
    // ... iterate all annoations ...
    for ( std::vector<QPDFAnnotationObjectHelper>::const_iterator annot_iter = a_annotations.begin(); annot_iter != a_annotations.end(); ++annot_iter) {
        // ... check if it's the annotation we're looking for ...
        QPDFFormFieldObjectHelper ffh = a_form.getFieldForAnnotation(*annot_iter);
        if ( "/Sig" == ffh.getFieldType() && a_name == ffh.getFullyQualifiedName() ) {
            // ... load range ...
            auto byte_range = ffh.getValue().getKey("/ByteRange");
            if ( QPDFObject::ot_array != byte_range.getTypeCode() || 4 != byte_range.getArrayNItems() ) {
                throw cc::Exception("%s - found but it's not a valid array!", sk_byte_range_err_msg_prefix_);
            }
            int idx = 0;
            for ( auto v : { &o_range.before_start_, &o_range.before_size_, &o_range.after_start_, &o_range.after_size_ } ) {
                (*v) = static_cast<size_t>(byte_range.getArrayItem(idx++).getNumericValue());
            }
            // ... found ...
            return true;
        }
    }
    // ... not found ...
    return false;
}
