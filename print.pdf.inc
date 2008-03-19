<?php
/* $Id$ */

require_once(drupal_get_path('module', 'print') .'/print.pages.inc');

function print_pdf_controller() {
  // Remove the printpdf/ prefix
  $args = substr($_GET['q'], strpos($_GET['q'], '/')+1);

  $print_pdf_settings = variable_get('print_pdf_settings', print_pdf_settings_default());

  $print = print_controller();
  // Img elements must be set to absolute
  $pattern = "@<(img\s[^>]*?)>@is";
  $print["content"] = preg_replace_callback($pattern, "_print_rewrite_urls", $print["content"]);
  $print["logo"] = preg_replace_callback($pattern, "_print_rewrite_urls", $print["logo"]);
  $print["footer_message"] = preg_replace_callback($pattern, "_print_rewrite_urls", $print["footer_message"]);

  if (basename($print_pdf_settings['pdf_tool']) == "dompdf_config.inc.php") {
    _print_pdf_dompdf($print, $args .".pdf");
  }
  elseif (basename($print_pdf_settings['pdf_tool']) == "tcpdf.php") {
    _print_pdf_tcpdf($print, $args .".pdf");
  }
  else {
    return drupal_not_found();
  }
}

function _print_pdf_dompdf($print, $filename, $directives = array()) {
  $print_pdf_settings = variable_get('print_pdf_settings', print_pdf_settings_default());
  require_once($print_pdf_settings['pdf_tool']);

  ob_start();
  include_once(_print_get_template($print["type"]));
  $html = ob_get_contents();
  ob_end_clean();

  $allowed_directives = array(
    'set_base_path',
    'set_host',
    'set_paper',
    'set_protocol',
  );

  $dompdf = new DOMPDF();
  $dompdf->load_html($html);
  
  foreach ($directives as $directive => $params) {
    if (in_array($directive, $allowed_directives)) {
      call_user_func_array(array($dompdf, $directive), $params);
    }
  }

  $dompdf->render();
  $dompdf->stream($filename, array("Attachment" => 0));
}

function _print_pdf_tcpdf($print, $filename) {
  $print_pdf_settings = variable_get('print_pdf_settings', print_pdf_settings_default());
  require_once($print_pdf_settings['pdf_tool']);

  $pdf = new TCPDF(PDF_PAGE_ORIENTATION, PDF_UNIT, PDF_PAGE_FORMAT, true);
  $pdf->Open();

  $pdf->SetAuthor($print["submitted"]);
  $pdf->SetCreator("Drupal");
  $pdf->SetKeywords(trim(strip_tags($print["taxonomy"])));
  $pdf->SetTitle($print["title"]);

  // set margins
  $pdf->SetMargins(PDF_MARGIN_LEFT, PDF_MARGIN_TOP, PDF_MARGIN_RIGHT);
  $pdf->SetHeaderMargin(PDF_MARGIN_HEADER);
  $pdf->SetFooterMargin(PDF_MARGIN_FOOTER);

  $pdf->SetHeaderData("", 0, $print["title"], strip_tags($print["site_name"]));
  
  // set auto page breaks
  $pdf->SetAutoPageBreak(TRUE, PDF_MARGIN_BOTTOM);
  $pdf->AliasNbPages();
  
  // set image scale factor
  $pdf->setImageScale(PDF_IMAGE_SCALE_RATIO);
  
  $pdf->setHeaderFont(Array(PDF_FONT_NAME_MAIN, '', PDF_FONT_SIZE_MAIN));
  $pdf->setFooterFont(Array(PDF_FONT_NAME_DATA, '', PDF_FONT_SIZE_DATA));

  $pdf->AddPage();

  $pdf->SetFont(PDF_FONT_NAME_MAIN, 'B',  PDF_FONT_SIZE_MAIN * K_TITLE_MAGNIFICATION);
  $pdf->SetLineWidth(1);
  $pdf->writeHTML($print["title"]);
  $pdf->Ln(5);

  if ($print["submitted"]) {
    $pdf->SetFont(PDF_FONT_NAME_MAIN, 'BI', PDF_FONT_SIZE_MAIN);
    $pdf->MultiCell(0, 5, $print["submitted"], 0, 'L', 0);
  }

  if ($print["created"]) {
    $pdf->SetFont('', '', PDF_FONT_SIZE_MAIN);
    $pdf->MultiCell(0, 5, $print["created"], 0, 'L', 0);
    $pdf->Ln(5);
  }

  $pdf->SetFont(PDF_FONT_NAME_MAIN, '', PDF_FONT_SIZE_MAIN);
  $html = preg_replace("/<head>.*<\/head>/sim", "", $html);
  $pdf->writeHTML($print["content"]);
  $pdf->Output($filename, 'I');
}
