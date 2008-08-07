<?php
// $Id$

/**
 * @file
 * Generates the PDF versions of the pages
 *
 * This file is included by the print_pdf module and includes the
 * functions that interface with the PDF generation packages.
 */

require_once(drupal_get_path('module', 'print') .'/print.pages.inc');

function print_pdf_controller() {
  $args = func_get_args();
  // Remove the printpdf/ prefix
  $path = implode('/', $args);
  $cid = isset($_GET['comment']) ? $_GET['comment'] : NULL;

  $print_pdf_pdf_tool = variable_get('print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT);

  $print = print_controller($path, $cid);
  // Img elements must be set to absolute
  $pattern = '!<(img\s[^>]*?)>!is';
  $print['content'] = preg_replace_callback($pattern, '_print_rewrite_urls', $print['content']);
  $print['logo'] = preg_replace_callback($pattern, '_print_rewrite_urls', $print['logo']);
  $print['footer_message'] = preg_replace_callback($pattern, '_print_rewrite_urls', $print['footer_message']);

  $node = $print['node'];
  ob_start();
  include_once(_print_get_template('pdf', $print['type']));
  $html = ob_get_contents();
  ob_end_clean();

  if (basename($print_pdf_pdf_tool) == 'dompdf_config.inc.php') {
    _print_pdf_dompdf($print, $html, $path .'.pdf');
  }
  elseif (basename($print_pdf_pdf_tool) == 'tcpdf.php') {
    _print_pdf_tcpdf($print, $html, $path .'.pdf');
  }
  else {
    return drupal_not_found();
  }
}

function _print_pdf_dompdf($print, $html, $filename) {
  $print_pdf_pdf_tool = variable_get('print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT);
  $print_pdf_paper_size = variable_get('print_pdf_paper_size', PRINT_PDF_PAPER_SIZE_DEFAULT);
  $print_pdf_page_orientation = variable_get('print_pdf_page_orientation', PRINT_PDF_PAGE_ORIENTATION_DEFAULT);
  $print_pdf_content_disposition = variable_get('print_pdf_content_disposition', PRINT_PDF_CONTENT_DISPOSITION_DEFAULT);
  require_once($print_pdf_pdf_tool);

  $url_array  = parse_url($print['url']);

  $protocol = $url_array['scheme'] .'://';
  $host = $url_array['host'];
  $path = dirname($url_array['path']) .'/';

  $dompdf = new DOMPDF();
  $dompdf->set_base_path($path);
  $dompdf->set_host($host);
  $dompdf->set_paper(strtolower($print_pdf_paper_size), $print_pdf_page_orientation);
  $dompdf->set_protocol($protocol);

  $dompdf->load_html($html);

  $dompdf->render();
  $dompdf->stream($filename, array('Attachment' => ($print_pdf_content_disposition == 2)));
}

function _print_pdf_tcpdf($print, $html, $filename) {
  global $base_url;
  $print_pdf_pdf_tool = variable_get('print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT);
  $print_pdf_paper_size = variable_get('print_pdf_paper_size', PRINT_PDF_PAPER_SIZE_DEFAULT);
  $print_pdf_page_orientation = variable_get('print_pdf_page_orientation', PRINT_PDF_PAGE_ORIENTATION_DEFAULT);
  $print_pdf_content_disposition = variable_get('print_pdf_content_disposition', PRINT_PDF_CONTENT_DISPOSITION_DEFAULT);

  define('K_TCPDF_EXTERNAL_CONFIG', TRUE);
  define('K_PATH_MAIN', realpath(dirname($print_pdf_pdf_tool)) .'/');
  define('K_PATH_URL', $base_url .'/'. dirname($print_pdf_pdf_tool) .'/');
  define('K_PATH_FONTS', K_PATH_MAIN .'fonts/');
  define('K_PATH_CACHE', K_PATH_MAIN .'cache/');
  define('K_PATH_IMAGES', K_PATH_MAIN .'images/');
  define('K_BLANK_IMAGE', K_PATH_IMAGES .'_blank.png');
  define('K_CELL_HEIGHT_RATIO', 1.25);
  define('K_SMALL_RATIO', 2/3);

  require_once($print_pdf_pdf_tool);
  require_once('print_pdf.class.inc');

  $font_name = 'dejavusans';
  $font_size = 10;

  $orientation = strtoupper($print_pdf_page_orientation{0});

  // create new PDF document
  $pdf = new PrintTCPDF($orientation , 'mm', $print_pdf_paper_size, TRUE);

  // set document information
  $pdf->SetAuthor(strip_tags($print['submitted']));
  $pdf->SetCreator(variable_get('site_name', 'Drupal'));
  $pdf->SetTitle($print['title']);
  $keys = implode(' ', explode("\n", trim(strip_tags($print['taxonomy']))));
  $pdf->SetKeywords($keys);
  $pdf->setPDFVersion('1.6');

  $logo_ret = preg_match('!src\s*=\s*(\'.*?\'|".*?"|[^\s]*)!i', $print['logo'], $matches);
  if ($logo_ret) {
    $logo = trim($matches[1], '\'"');
    $path_parts = pathinfo($logo);
    $newfilename = 'print_pdf_logo_'. user_password(8) .'.'. $path_parts['extension'];
    copy($logo, K_PATH_IMAGES . $newfilename);
    $size = getimagesize(K_PATH_IMAGES . $newfilename);
    $ratio = $size[0] / $size[1];
  }
  else {
    $newfilename = '';
    $ratio = 0;
  }

  // set header data
  $pdf->SetHeaderData($newfilename, 10 * $ratio, $print['title'], strip_tags($print['site_name']));

  // set footer data
  $pdf->SetFooterData($print['footer_message']);

  // set header and footer fonts
  $pdf->setHeaderFont(Array($font_name, '', $font_size));
  $pdf->setFooterFont(Array($font_name, '', 8));

  $pdf->SetFont($font_name, '', $font_size);

  // set margins
  $pdf->SetMargins(15, 27, 15);
  $pdf->SetHeaderMargin(5);
  $pdf->SetFooterMargin(10);

  // set auto page breaks
  $pdf->SetAutoPageBreak(TRUE, 25);

  // set image scale factor
  $pdf->setImageScale(4);

  // set image compression quality
  $pdf->setJPEGQuality(100);

  //initialize document
  $pdf->AliasNbPages();

  // add a page
  $pdf->AddPage();

  preg_match('!<body.*?>(.*)</body>!sim', $html, $matches);
  $pattern = "!(?:<div>{$print['logo']}</div>|<div.*?>{$print['footer_message']}</div>|<div class=\"print-(?:site_name|breadcrumb)\">.*?</div>|<hr class=\"print-hr\" />)!i";
  $matches[1] = preg_replace($pattern, '', $matches[1]);

  $paragraphs = preg_split('!(<p\s*/?>|</p>)!i', $matches[1], -1, PREG_SPLIT_NO_EMPTY);
  foreach($paragraphs as $value) {
    $par = trim($value);
    if (!empty($par)) {
      $pdf->writeHTML($par .'<br />');
    }
  }

  // reset pointer to the last page
  $pdf->lastPage();

  //Close and output PDF document
  $output_dest = ($print_pdf_content_disposition == 2) ? 'D' : 'I';
  $pdf->Output($filename, $output_dest);
  if (!empty($newfilename)) {
    unlink(K_PATH_IMAGES . $newfilename);
  }
}
