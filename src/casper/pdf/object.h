/**
 * @file object.h
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
#ifndef CASPER_PDF_OBJECT_H_
#define CASPER_PDF_OBJECT_H_

#include "cc/non-movable.h"

#include <string>

namespace casper
{

    namespace pdf
    {
    
        class Object : public ::cc::NonMovable
        {
            
        public: // Const Data
            
            const std::string name_;

        public: // Constructor(s) / Destructor
            
            Object () = delete;
            Object (const std::string& a_name);
            Object (const Object& a_object);
            
            virtual ~Object ();
            
        public: // Method(s) / Function(s)
            
            double ToPoints (double a_value, const char* const a_units) const;
            
        public: // Overloaded Operator(s)
            
            void operator = (Object const&)  = delete;  // assignment is not allowed
            
        }; // end of class 'Object'

    } // end of namespace 'pdf'

} // end of namespace 'casper'

#endif // CASPER_PDF_OBJECT_H_
