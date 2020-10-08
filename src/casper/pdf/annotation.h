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
#ifndef CASPER_PDF_ANNOTATION_H_
#define CASPER_PDF_ANNOTATION_H_

#include "casper/pdf/object.h"

#include "casper/pdf/types.h"

#include <string>

namespace casper
{

    namespace pdf
    {
    
        // MARK: -

        class Annotation : public Object
        {
            
        public: // Data Type(s)
                        
            typedef struct {
                size_t x_;
                size_t y_;
                size_t w_;
                size_t h_;
            } Rect;
                        
        private: // Data
            
            Rect   rect_;
            size_t page_;

        public: // Constructor(s) / Destructor
            
            Annotation () = delete;
            Annotation (const std::string& a_name);
            Annotation (const Annotation& a_annotation);
            
            virtual ~Annotation ();
            
        public: // Method(s) / Function(s)
            
            void Set (const Rect& a_rect, const size_t a_page);

            const Rect&   rect () const;
            const size_t& page () const;
        
        }; // end of class 'Annotation'

        inline void Annotation::Set (const Rect& a_rect, const size_t a_page)
        {
            rect_ = a_rect;
            page_ = a_page;
        }

        inline const Annotation::Rect& Annotation::rect () const
        {
            return rect_;
        }

        inline const size_t& Annotation::page () const
        {
            return page_;
        }

        // MARK: -

        class SignatureAnnotation : public Annotation
        {
            
        public: // Data Type(s)
            
            typedef ::casper::pdf::ByteRange     ByteRange;
            typedef ::casper::pdf::SignatureInfo SignatureInfo;
            
        private: // Data

            SignatureInfo info_;
            ByteRange     byte_range_;
            bool          visible_;

        public: // Constructor(s) / Destructor
            
            SignatureAnnotation () = delete;
            SignatureAnnotation (const std::string& a_name);
            SignatureAnnotation (const SignatureAnnotation& a_annotation);
            
            virtual ~SignatureAnnotation ();
            
        public: // Method(s) / Function(s)
            
            void Set (const Rect& a_rect, const size_t a_page, const bool a_visible);
            void Set (const SignatureInfo& a_info);
            void Set (const ByteRange& a_range);

            const SignatureInfo& info       () const;
            const ByteRange&     byte_range () const;
            const bool&          visible    () const;

        }; // end of class 'SignatureAnnotation'

        /**
         * @brief Set rectangle, page and visibility flag properties.
         *
         * @param a_rect    See \link Rect \link.
         * @param a_page    Page number, 1..n
         * @param a_visible True if should be visible, false otherwise.
         */
        inline void SignatureAnnotation::Set (const Rect& a_rect, const size_t a_page, const bool a_visible)
        {
            Annotation::Set(a_rect, a_page);
            visible_ = a_visible;
        }

        /**
         * @brief Set signature info fields.
         *
         * @param a_info See \link SignatureInfo \link.
         */
        inline void SignatureAnnotation::Set (const SignatureInfo& a_info)
        {
            info_ = a_info;
        }
    
        /**
         * @brief Set signature field byte range.
         *
         * @param a_range See \link ByteRange \link.
         */
        inline void SignatureAnnotation::Set (const SignatureAnnotation::ByteRange& a_range)
        {
            byte_range_ = a_range;
        }

        /**
         * @return R/O access to \link SignatureInfo \link.
         */
        inline const SignatureInfo& SignatureAnnotation::info () const
        {
            return info_;
        }
    
        /**
         * @return R/O access to \link ByteRange \link.
         */
        inline const SignatureAnnotation::ByteRange& SignatureAnnotation::byte_range () const
        {
            return byte_range_;
        }

        /**
         * @return R/O access to visibility flag.
         */
        inline const bool& SignatureAnnotation::visible () const
        {
            return visible_;
        }
    
    } // end of namespace 'pdf'

} // end of namespace 'casper'

#endif // CASPER_PDF_ANNOTATION_H_
