<?php
/* $Id$ */

require_once(drupal_get_path('module', 'print') .'/print.pages.inc');

function print_pdf_controller() {
  // Remove the printpdf/ prefix
  $path = substr($_GET['q'], strpos($_GET['q'], '/')+1);
  $cid = isset($_GET['comment']) ? $_GET['comment'] : NULL;

  $print_pdf_settings = variable_get('print_pdf_settings', print_pdf_settings_default());

  $print = print_controller($path, $cid);
  // Img elements must be set to absolute
  $pattern = "@<(img\s[^>]*?)>@is";
  $print["content"] = preg_replace_callback($pattern, "_print_rewrite_urls", $print["content"]);
  $print["logo"] = preg_replace_callback($pattern, "_print_rewrite_urls", $print["logo"]);
  $print["footer_message"] = preg_replace_callback($pattern, "_print_rewrite_urls", $print["footer_message"]);

  ob_start();
  include_once(_print_get_template("pdf", $print["type"]));
  $html = ob_get_contents();
  ob_end_clean();

  if (basename($print_pdf_settings['pdf_tool']) == "dompdf_config.inc.php") {
    _print_pdf_dompdf($print, $html, $path .".pdf");
  }
  elseif (basename($print_pdf_settings['pdf_tool']) == "tcpdf.php") {
    _print_pdf_tcpdf($print, $html, $path .".pdf");
  }
  else {
    return drupal_not_found();
  }
}

function _print_pdf_dompdf($print, $html, $filename) {
  $print_pdf_settings = variable_get('print_pdf_settings', print_pdf_settings_default());
  require_once($print_pdf_settings['pdf_tool']);

  $url_array  = parse_url($print['url']);

  $protocol = $url_array['scheme'] . "://";
  $host = $url_array['host'];
  $path = dirname($url_array['path']) . "/";

  $dompdf = new DOMPDF();
  $dompdf->set_base_path($path);
  $dompdf->set_host($host);
  $dompdf->set_paper(strtolower($print_pdf_settings['paper_size']), $print_pdf_settings['page_orientation']);
  $dompdf->set_protocol($protocol);

  $dompdf->load_html($html);
  
  $dompdf->render();
  $dompdf->stream($filename, array("Attachment" => ($print_pdf_settings['content_disposition'] == 2)));
}

function _print_pdf_tcpdf($print, $html, $filename) {
  $print_pdf_settings = variable_get('print_pdf_settings', print_pdf_settings_default());
  require_once($print_pdf_settings['pdf_tool']);

  $font_name = "dejavusans";
  $font_size = 10;

  $orientation = strtoupper($print_pdf_settings['page_orientation']{0});

  // create new PDF document
  $pdf = new TCPDF($orientation , PDF_UNIT, $print_pdf_settings['paper_size'], TRUE);

  // set document information
  $pdf->SetAuthor(strip_tags($print["submitted"]));
  $pdf->SetCreator(variable_get("site_name", "Drupal"));
  $pdf->SetTitle($print["title"]);
  $keys = implode(" ", explode("\n", trim(strip_tags($print["taxonomy"]))));
  $pdf->SetKeywords($keys);

  // set header data
  $pdf->SetHeaderData("", 0, $print["title"], strip_tags($print["site_name"]));

  // set header and footer fonts
  $pdf->setHeaderFont(Array($font_name, '', $font_size));
  $pdf->setFooterFont(Array($font_name, '', PDF_FONT_SIZE_DATA));

  // set margins
  $pdf->SetMargins(PDF_MARGIN_LEFT, PDF_MARGIN_TOP, PDF_MARGIN_RIGHT);
  $pdf->SetHeaderMargin(PDF_MARGIN_HEADER);
  $pdf->SetFooterMargin(PDF_MARGIN_FOOTER);
  
  // set auto page breaks
  $pdf->SetAutoPageBreak(TRUE, PDF_MARGIN_BOTTOM);

  // set image scale factor
  $pdf->setImageScale(PDF_IMAGE_SCALE_RATIO);

  //initialize document
  $pdf->AliasNbPages();
  
  // add a page
  $pdf->AddPage();

  preg_match("/<body>(.*)<\/body>/sim", $html, $matches);
  $matches[1] = preg_replace("/.*?(<[^<]*?print-content.*?>)/sim", "$1", $matches[1]);

  $pdf->SetFont($font_name, 'B', $font_size * K_TITLE_MAGNIFICATION);
  $pdf->writeHTML($print["title"]);

  if ($print["submitted"]) {
    $pdf->Ln(2);
    $pdf->SetFont($font_name, 'B', $font_size);
    $pdf->writeHTML($print["submitted"]);
    $pdf->SetFont($font_name, '', $font_size);
    $pdf->writeHTML($print["created"]);
  }

  $pdf->SetFont($font_name, '', $font_size);
  $pdf->writeHTML($matches[1]);

  //Close and output PDF document 
  $output_dest = ($print_pdf_settings['content_disposition'] == 2) ? 'D' : 'I';
  $pdf->Output($filename, $output_dest);
}
