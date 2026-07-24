<?php

/**
 * @file
 * Generates the PDF version using dompdf.
 *
 * This file is included by the print_pdf_dompdf module and includes the
 * functions that interface with the dompdf library.
 *
 * @ingroup print
 */

/**
 * Implements hook_print_pdf_generate().
 */
function print_pdf_dompdf_print_pdf_generate($html, $meta, $paper_size = NULL, $page_orientation = NULL) {
  $config = config('print_pdf_dompdf.settings');
  $print_pdf_config = config('print_pdf.settings');
  module_load_include('inc', 'print', 'includes/print');

  $pdf_tool = explode('|', $print_pdf_config->get('print_pdf_pdf_tool'));
  if (empty($paper_size)) {
    $paper_size = $print_pdf_config->get('print_pdf_paper_size');
  }
  if (empty($page_orientation)) {
    $page_orientation = $print_pdf_config->get('print_pdf_page_orientation');
  }
  $images_via_file = $print_pdf_config->get('print_pdf_images_via_file');
  $unicode = TRUE;

  if ($print_pdf_config->get('print_pdf_autoconfig')) {
    $font_subsetting = $config->get('print_pdf_dompdf_font_subsetting');
    $unicode = $config->get('print_pdf_dompdf_unicode');

    if (!defined('DOMPDF_ENABLE_PHP')) {
      define('DOMPDF_ENABLE_PHP', FALSE);
    }
    if (!defined('DOMPDF_ENABLE_REMOTE')) {
      define('DOMPDF_ENABLE_REMOTE', TRUE);
    }
    if (!defined('DOMPDF_TEMP_DIR')) {
      define('DOMPDF_TEMP_DIR', file_directory_temp());
    }
    if (!defined('DOMPDF_UNICODE_ENABLED')) {
      define('DOMPDF_UNICODE_ENABLED', $unicode);
    }
    if (!defined('DOMPDF_ENABLE_FONTSUBSETTING')) {
      define('DOMPDF_ENABLE_FONTSUBSETTING', $font_subsetting);
    }
    if (!defined('DOMPDF_FONT_CACHE')) {
      define('DOMPDF_FONT_CACHE', backdrop_realpath('public://print_pdf/print_pdf_dompdf/fonts/'));
    }
  }

  $tool_path = dirname($pdf_tool[1]) . '/../autoload.inc.php';
  // When installed via composer, or downloaded the zipfile that includes an
  // autoload file inside a directory that Print module scans, such as
  // ./libraries.
  $autoload_path = dirname($pdf_tool[1]) . '/../../../autoload.php';
  $composer_path = dirname($pdf_tool[1]) . '/../../../composer';
  if (file_exists($tool_path)) {
    require_once $tool_path;
  } elseif (module_exists('xautoload') && is_dir($composer_path)) {
    xautoload()->adapter->composerDir($composer_path);
  } elseif (file_exists($autoload_path)) {
    require_once $autoload_path;
  } else {
    watchdog('print_pdf', 'Configured PDF tool does not exist at path: %path', array('%path' => $tool_path), WATCHDOG_ERROR);
    throw new Exception("Configured PDF tool does not exist, unable to generate PDF.");
  }

  $options = new \Dompdf\Options();
  if ($print_pdf_config->get('print_pdf_autoconfig')) {
    $options->set('isPhpEnabled', FALSE);
    $options->set('isRemoteEnabled', TRUE);
    $options->set('tempDir', file_directory_temp());
    $options->set('isFontSubsettingEnabled', $font_subsetting);
    $options->set('fontCache', backdrop_realpath('public://print_pdf/print_pdf_dompdf/fonts/'));
  }
  $unicode = TRUE;

  // Allow third-party modules to alter the dompdf object.
  backdrop_alter('print_pdf_dompdf', $dompdf);

  // Try to use local file access for image files.
  $html = _print_access_images_via_file($html, $images_via_file);
  // Remove all scripts due to security concerns.
  $html = preg_replace('!<script(.*?)>(.*?)</script>!is', '', $html);

  // Spaces in img URLs must be replaced with %20, when using external access.
  if (!$images_via_file) {
    $pattern = '!<(img\s[^>]*?)>!is';
    $html = preg_replace_callback($pattern, '_print_replace_spaces', $html);
  }

  // It seems dompdf has problems with something in system.css, don't use it.
  $html = preg_replace('!<link.*?modules/system/system.css.*?/>!', '', $html);

  $url_array = parse_url($meta['url']);

  $protocol = $url_array['scheme'] . '://';
  $host = $url_array['host'];
  $path = dirname($url_array['path']) . '/';

  $options->set('allowedRemoteHosts', [$host]);
  $options->set('defaultPaperSize', backdrop_strtolower($paper_size));
  $options->set('defaultPaperOrientation', $page_orientation);
  $options->set('allowedProtocols', [$protocol]);
  $dompdf = new \Dompdf\Dompdf($options);
  $dompdf->setBasePath($path);

  // If dompdf Unicode support is disabled, try to convert to ISO-8859-1 and
  // then to HTML entities.
  if (!$unicode) {
    // Convert the euro sign to an HTML entity.
    $html = str_replace('€', '&#0128;', $html);

    // Convert from UTF-8 to ISO 8859-1 and then to HTML entities.
    if (function_exists('utf8_decode')) {
      $html = utf8_decode($html);
    } elseif (function_exists('mb_convert_encoding')) {
      $html = mb_convert_encoding($html, 'ISO-8859-1', 'UTF-8');
    }
    $html = htmlspecialchars_decode(htmlentities($html, ENT_NOQUOTES, 'ISO-8859-1'), ENT_NOQUOTES);
  } else {
    // Otherwise, ensure the content is properly formatted Unicode.
    $html = mb_convert_encoding($html, 'HTML-ENTITIES', 'UTF-8');
  }

  // Must get rid of tbody (dompdf goes into recursion).
  $html = preg_replace('!<tbody[^>]*?>|</tbody>!i', '', $html);

  $dompdf->loadHtml($html);
  $dompdf->render();
  return $dompdf->output();
}
