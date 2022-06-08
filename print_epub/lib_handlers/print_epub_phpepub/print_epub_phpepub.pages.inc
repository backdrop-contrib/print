<?php

/**
 * @file
 * Generates the EPUB version using PHPePub.
 *
 * This file is included by the print_epub_phpepub module and includes the
 * functions that interface with the PHPePub library.
 *
 * @ingroup print
 */

/**
 * Implements hook_print_epub_generate().
 */
function print_epub_phpepub_print_epub_generate($html, $meta, $filename = NULL) {
  global $language, $base_url;

  module_load_include('inc', 'print', 'includes/print');

  $epub_tool = explode('|', config_get('print_epub.settings', 'print_epub_epub_tool'));
  $images_via_file = config_get('print_epub.settings', 'print_epub_images_via_file');

  $tool_path = BACKDROP_ROOT . '/' . $epub_tool[1];
  if (file_exists($tool_path)) {
    require_once $tool_path;
  }
  else {
    watchdog('print_epub', 'Configured EPUB tool does not exist at path: %path', array('%path' => $tool_path), WATCHDOG_ERROR);
    throw new Exception("Configured EPUB tool does not exist, unable to generate EPUB.");
  }

  // Try to use local file access for image files.
  $html = _print_access_images_via_file($html, $images_via_file);
  $version = _print_epub_phpepub_version($epub_tool[1]);

  // Set document information.
  if (version_compare($version, '4.0.0', '>=')) {
    $epub = new PHPePub Core EPub();
  }
  else {
    $epub = new EPub();
  }

  $epub->setTitle(html_entity_decode($meta['title'], ENT_QUOTES, 'UTF-8'));
  $epub->setIdentifier($meta['url'], $epub::IDENTIFIER_URI);
  $epub->setLanguage($language->language);
  if (isset($meta['name'])) {
    $epub->setAuthor(strip_tags($meta['name']), strip_tags($meta['name']));
  }
  $epub->setPublisher(config_get('system.performance', 'site_name'), $base_url);
  $epub->setSourceURL($meta['url']);

  @$epub->addChapter("Chapter", "epub.html", $html, FALSE);

  // Finalize the book, and build the archive.
  $epub->finalize();

  // Close and output EPUB document.
  $epub->sendBook(empty($filename) ? 'page' : $filename);
  return TRUE;
}
