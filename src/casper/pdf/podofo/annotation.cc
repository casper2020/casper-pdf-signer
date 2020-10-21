/**
 * @file annotation.cc
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

#include "casper/pdf/podofo/annotation.h"

#include "cc/exception.h"

// MARK: - SignatureAnnotation

/**
 * @brief Partial copy constructor.
 *
 * @param a_annotation Original annotation
 */
casper::pdf::podofo::SignatureAnnotation::SignatureAnnotation (const ::casper::pdf::SignatureAnnotation& a_annotation)
    : casper::pdf::SignatureAnnotation(a_annotation)
{
    /* empty */
}

/**
 * @brief Destructor.
 */
casper::pdf::podofo::SignatureAnnotation::~SignatureAnnotation ()
{
    /* empty */
}

// MARK: -

/**
 * @brief Draw a signature annotation.
 *
 * @param a_annotation Annotation to draw.
 * @param a_rect       Annotation bbox.
 * @param a_document   Document where annotation is placed at.
 * @param a_field      Signature field.
 */
void casper::pdf::podofo::SignatureAnnotation::Draw (const ::PoDoFo::PdfAnnotation& a_annotation, const ::PoDoFo::PdfRect& a_rect,
                                                     ::PoDoFo::PdfDocument& a_document, ::PoDoFo::PdfSignatureField& a_field) const
{
    ::PoDoFo::PdfPainter painter;

    try {

        ::PoDoFo::PdfRect    annotSize( 0.0, 0.0, a_rect.GetWidth(), a_rect.GetHeight() );
        ::PoDoFo::PdfXObject sigXObject( annotSize, &a_document );

        painter.SetPage(&sigXObject);

        ::PoDoFo::PdfFont* font = a_document.CreateFont(fonts().default_.id_.c_str(),
                                                        /* bBold              */ false,
                                                        /* bItalic            */ false,
                                                        /* bSymbolCharset     */ false,
                                                        /* pEncoding          */ PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(),
                                                        /* eFontCreationFlags */ PoDoFo::PdfFontCache::eFontCreationFlags_AutoSelectBase14,
                                                        /* bEmbedd            */ true,
                                                        /* pszFileName        */ fonts().default_.uri_.c_str()
        ); // NOTES: owned and release by 'a_document'
                
        /*
         * FROM PoDoFo example:
         * Workaround Adobe's reader error 'Expected a dict object.' when the stream
         * contains only one object which does Save()/Restore() on its own, like
         * the image XObject.
         */
        painter.Save();
        painter.Restore();

        // ... Y AXE IS FLIPPED ...
        
        //
        // Draw 'frame':
        //
        painter.SetStrokeStyle(::PoDoFo::EPdfStrokeStyle::ePdfStrokeStyle_Solid);
        painter.SetStrokeWidth(1.0);
        painter.SetStrokingColor(0.78, 0.78, 0.78); // #C7C7C7
        painter.Rectangle(0.0, 0.0, annotSize.GetWidth(), annotSize.GetHeight());
        painter.Stroke();

        //
        // Draw 'logo':
        //
        ::PoDoFo::PdfImage image = ::PoDoFo::PdfImage(&a_document);
                
        image.LoadFromFile(images().logo_.uri_.c_str());
        
        const double sx = ( 59.0 / image.GetWidth()  );
        const double sy = ( 59.0 / image.GetHeight() );

        painter.DrawImage(4.0, 4.0, &image, sx, sy);

        //
        // Draw Text:
        //
        painter.SetColor(0.58, 0.58, 0.58); // #969696
        font->SetFontSize(6);
        painter.SetFont(font);
        
        // x =
        painter.DrawText(72.0, ToPoints(2, "mm"), ::PoDoFo::PdfString("Assinado por: <TODO>"));
        painter.DrawText(72.0, ToPoints(10, "mm"), ::PoDoFo::PdfString("Data:  <TODO>"));
        
        a_field.SetAppearanceStream(&sigXObject);

        //
        // Finish
        //
        painter.FinishPage();

    } catch (::PoDoFo::PdfError& e ) {
        // ... call finish?
        if ( nullptr != painter.GetPage() ) {
            try {
                painter.FinishPage();
            } catch( ... ) {
                // ... eat it ...
            }
        }
        // ... report error ...
        throw cc::Exception("PoDoFo: %s", e.what());
    }
}
