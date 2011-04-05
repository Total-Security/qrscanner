/*
 *  main.cpp
 *  zxing
 *
 *  Copyright 2010 ZXing authors All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <Magick++.h>
#include "MagickBitmapSource.h"
#include <zxing/common/Counted.h>
//#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/Binarizer.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/Result.h>
#include <zxing/ReaderException.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/common/HybridBinarizer.h>
#include <exception>
#include <zxing/Exception.h>
#include <zxing/common/IllegalArgumentException.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/DecodeHints.h>

//#include <zxing/qrcode/detector/Detector.h>
//#include <zxing/qrcode/detector/QREdgeDetector.h>
//#include <zxing/qrcode/decoder/Decoder.h>

using namespace Magick;
using namespace std;
using namespace zxing;
//using namespace zxing::qrcode;

//static bool raw_dump = false;
//static bool show_format = false;
static bool tryHarder = true;
//static bool show_filename = false;

static const int MAX_EXPECTED = 1024;

Ref<Result> decode(Ref<BinaryBitmap> image, DecodeHints hints) {
  Ref<Reader> reader(new MultiFormatReader);
  return reader->decode(image, hints);
}


extern "C" char *decode_qr_image(const char *fname) {

  string cell_result;
  int res = -1;

  Ref<BitMatrix> matrix(NULL);
  Ref<Binarizer> binarizer(NULL);
  const char* result_format = "";

    Image image;
    image.density(Geometry(300,300));  // for PDFs or vector input with low, incorrect DPI set eg. some Canon scanners
    try {
      image.read(fname);
    } catch (...) {
      cerr << "Unable to open image, ignoring" << endl;
      return(NULL);
    }


  try {
    Ref<MagickBitmapSource> source(new MagickBitmapSource(image));

//    binarizer = new HybridBinarizer(source);
    binarizer = new GlobalHistogramBinarizer(source);

    DecodeHints hints(DecodeHints::BARCODEFORMAT_QR_CODE_HINT /*DecodeHints::DEFAULT_HINT*/);
    hints.setTryHarder(tryHarder);
    Ref<BinaryBitmap> binary(new BinaryBitmap(binarizer));
    Ref<Result> result(decode(binary, hints));
    cell_result = result->getText()->getText();
    result_format = barcodeFormatNames[result->getBarcodeFormat()];
    res = 0;
  } catch (ReaderException e) {
    cell_result = "zxing::ReaderException: " + string(e.what());
    res = -2;
  } catch (zxing::IllegalArgumentException& e) {
    cell_result = "zxing::IllegalArgumentException: " + string(e.what());
    res = -3;
  } catch (zxing::Exception& e) {
    cell_result = "zxing::Exception: " + string(e.what());
    res = -4;
  } catch (std::exception& e) {
    cell_result = "std::exception: " + string(e.what());
    res = -5;
  }

  if(res<0)
    return(NULL);

  return(strdup(cell_result.c_str()));
}

/*
int main(int argc, char** argv) {
  if (argc <= 1) {
    cout << "Usage: " << argv[0] << " [--dump-raw] [--show-format] [--try-harder] [--show-filename] <filename1> [<filename2> ...]" << endl;
    return 1;
  }

  int total = 0;
  int gonly = 0;
  int honly = 0;
  int both = 0;
  int neither = 0;

  if (argc == 2) raw_dump = true;

  for (int i = 1; i < argc; i++) {
    string infilename = argv[i];
    if (infilename.substr(infilename.length()-3,3).compare("txt") == 0) {
      continue;
    }
    if (infilename.compare("--dump-raw") == 0) {
      raw_dump = true;
      continue;
    }
    if (infilename.compare("--show-format") == 0) {
      show_format = true;
      continue;
    }
    if (infilename.compare("--try-harder") == 0) {
      tryHarder = true;
      continue;
    }
    if (infilename.compare("--show-filename") == 0) {
      show_filename = true;
      continue;
    }
    if (!raw_dump)
      cerr << "Processing: " << infilename << endl;
    if (show_filename)
      cout << infilename << " ";
    Image image;
    try {
      image.read(infilename);
    } catch (...) {
      cerr << "Unable to open image, ignoring" << endl;
      continue;
    }

    string expected;
    expected = get_expected(infilename);

    int gresult = 1;
    int hresult = 1;

    hresult = test_image_hybrid(image, expected);
    gresult = test_image_global(image, expected);

    gresult = gresult == 0;
    hresult = hresult == 0;

    gonly += gresult && !hresult;
    honly += hresult && !gresult;
    both += gresult && hresult;
    neither += !gresult && !hresult;
    total = total + 1;
  }

  if (!raw_dump)
    cout << (honly+both)  << " passed hybrid, " << (gonly+both) << " passed global, "
      << both << " pass both, " << neither << " pass neither, " << honly
      << " passed only hybrid, " << gonly << " passed only global, of " << total
      << " total." << endl;

  return 0;
}
*/

