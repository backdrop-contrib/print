<?php

/**
 * @file
 * Generates the PDF version using mPDF.
 *
 * This file is included by the print_pdf_mpdf module and includes the
 * functions that interface with the mPDF library.
 *
 * @ingroup print
 */

/**
 * Implements hook_print_pdf_generate().
 */
function print_pdf_mpdf_print_pdf_generate($html, $meta, $paper_size = NULL, $page_orientation = NULL) {
  module_load_include('inc', 'print', 'includes/print');

  $pdf_tool = explode('|', config_get('print_pdf.settings', 'print_pdf_pdf_tool'));
  // Version 7 of the mpdf library uses a composer autoloader.
  // Also there no longer is way to truly detect the library version, so this
  // seems like the best alternative.
  $mpdf_version_7_plus = strpos($pdf_tool[1], 'autoload.php') !== FALSE;
  if (empty($paper_size)) {
    $paper_size = config_get('print_pdf.settings','print_pdf_paper_size');
  }
  if (empty($page_orientation)) {
    $page_orientation = config_get('print_pdf.settings','print_pdf_page_orientation');
  }
  $images_via_file = config_get('print_pdf.settings','print_pdf_images_via_file');

  $config = array();
  if ($mpdf_version_7_plus) {
    $config['tempDir'] = backdrop_realpath('public://print_pdf/print_pdf_mpdf/');
  }
  else {
    // Deprecated since mpdf v7.x.
    if (config_get('print_pdf.settings','print_pdf_autoconfig')) {
      if (!defined('_MPDF_TTFONTDATAPATH')) {
        define('_MPDF_TTFONTDATAPATH', backdrop_realpath('public://print_pdf/print_pdf_mpdf/ttfontdata/'));
      }
      if (!defined('_MPDF_TEMP_PATH')) {
        define('_MPDF_TEMP_PATH', backdrop_realpath('public://print_pdf/print_pdf_mpdf/tmp/'));
      }
    }
  }

  $tool_path = BACKDROP_ROOT . '/' . $pdf_tool[1];
  if (file_exists($tool_path)) {
    require_once $tool_path;
  }
  else {
    watchdog('print_pdf', 'Configured PDF tool does not exist at path: %path', array('%path' => $tool_path), WATCHDOG_ERROR);
    throw new Exception("Configured PDF tool does not exist, unable to generate PDF.");
  }

  $format = ($page_orientation == "landscape") ? $paper_size . "-L" : $paper_size;

  // Try to use local file access for image files.
  $html = _print_access_images_via_file($html, $images_via_file);

  // Set document information.
  if ($mpdf_version_7_plus) {
    $config['mode'] = 'utf-8';
    $config['format'] = $format;
    $mpdf = new Mpdf Mpdf($config);
  }
  else {
    $mpdf = new mPDF('UTF-8', $format);
  }

  if (isset($meta['name'])) {
    $mpdf->SetAuthor(strip_tags($meta['name']));
  }
  $mpdf->SetCreator(config_get('system.core', 'site_name'));
  /*
   // Pulled from the HTML meta data
   $mpdf->SetTitle(html_entity_decode($meta['title'], ENT_QUOTES, 'UTF-8'));

   $keys = implode(' ', explode("\n", trim(strip_tags($print['taxonomy']))));
   $mpdf->SetKeywords($keys);

   // Encrypt the file and grant permissions to the user to copy and print
   // No password is required to open the document
   // Owner has full rights using the password "MyPassword"
   $mpdf->SetProtection(array('copy', 'print'), '', 'MyPassword');
   $mpdf->SetProtection(array('copy', 'print', 'print-highres'), '', '');
   */
  // Verify if footer is set and configured.
  preg_match('!<div class="print-footer">(.*?)</div>!si', $html, $tpl_footer);
  if (!empty($tpl_footer[1]) && config_get('print.settings', 'print_footer_options')) {
    $footer = trim(preg_replace('!</?div[^>]*?>!i', '', $tpl_footer[1]));
    // Remove footer from html because it's not in footer region of the page.
    $html = preg_replace('!<div class="print-footer">(.*?)</div>!si', '', $html);
    $mpdf->setFooter($footer);
  }
  backdrop_alter('print_pdf_mpdf', $mpdf, $html, $meta);

  $mpdf->WriteHTML($html);

  // Try to recover from any warning/error.
  ob_clean();

  return $mpdf->Output('', 'S');
}
