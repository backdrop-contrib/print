<?php

/**
 * @file
 * Contains the administrative functions of the PDF version module.
 *
 * This file is included by the PDF version module, and includes the
 * settings form.
 *
 * @ingroup print
 */

/**
 * Form constructor for the PDF version module settings form.
 *
 * @see _print_pdf_tools()
 * @ingroup forms
 */
function print_pdf_settings() {
  $pdf_tools = _print_pdf_tools();
  $current_pdf_tool = variable_get('print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT);
  $pdf_tool_default = (in_array($current_pdf_tool, $pdf_tools)) ? $current_pdf_tool : PRINT_PDF_PDF_TOOL_DEFAULT;

  if ($pdf_tools != -1) {
    $link = print_pdf_print_link();

    $form['settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('PDF options'),
    );

    $form['settings']['print_pdf_pdf_tool'] = array(
      '#type' => 'radios',
      '#title' => t('PDF generation tool'),
      '#options' => $pdf_tools,
      '#default_value' => $pdf_tool_default,
      '#description' => t('This option selects the PDF generation tool being used by this module to create the PDF version.'),
    );

    $form['settings']['print_pdf_content_disposition'] = array(
      '#type' => 'radios',
      '#title' => t('Open PDF in'),
      '#options' => array(t('Same browser window'), t('New browser window'), t('Save dialog')),
      '#default_value' => variable_get('print_pdf_content_disposition', PRINT_PDF_CONTENT_DISPOSITION_DEFAULT),
      '#description' => t("Select the desired method for opening the PDF in the user's browser."),
    );

    $form['settings']['print_pdf_paper_size'] = array(
      '#type' => 'select',
      '#title' => t('Paper size'),
      '#options' => array('4A0' => '4A0', '2A0' => '2A0', 'A0' => 'A0',
                          'A1' => 'A1', 'A2' => 'A2', 'A3' => 'A3', 'A4' => 'A4',
                          'A5' => 'A5', 'A6' => 'A6', 'A7' => 'A7', 'A8' => 'A8',
                          'A9' => 'A9', 'A10' => 'A10', 'B0' => 'B0', 'B1' => 'B1',
                          'B2' => 'B2', 'B3' => 'B3', 'B4' => 'B4', 'B5' => 'B5',
                          'B6' => 'B6', 'B7' => 'B7', 'B8' => 'B8', 'B9' => 'B9',
                          'B10' => 'B10', 'C0' => 'C0', 'C1' => 'C1', 'C2' => 'C2',
                          'C3' => 'C3', 'C4' => 'C4', 'C5' => 'C5', 'C6' => 'C6',
                          'C7' => 'C7', 'C8' => 'C8', 'C9' => 'C9', 'C10' => 'C10',
                          'RA0' => 'RA0', 'RA1' => 'RA1', 'RA2' => 'RA2',
                          'RA3' => 'RA3', 'RA4' => 'RA4', 'SRA0' => 'SRA0',
                          'SRA1' => 'SRA1', 'SRA2' => 'SRA2', 'SRA3' => 'SRA3',
                          'SRA4' => 'SRA4', 'LETTER' => 'Letter', 'LEGAL' => 'Legal',
                          'EXECUTIVE' => 'Executive', 'FOLIO' => 'Folio',
                    ),
      '#default_value' => variable_get('print_pdf_paper_size', PRINT_PDF_PAPER_SIZE_DEFAULT),
      '#description' => t('Choose the paper size of the generated PDF.'),
    );

    $form['settings']['print_pdf_page_orientation'] = array(
      '#type' => 'select',
      '#title' => t('Page orientation'),
      '#options' => array('portrait' => t('Portrait'), 'landscape' => t('Landscape')),
      '#default_value' => variable_get('print_pdf_page_orientation', PRINT_PDF_PAGE_ORIENTATION_DEFAULT),
      '#description' => t('Choose the page orientation of the generated PDF.'),
    );
    $form['settings']['print_pdf_images_via_file'] = array(
      '#type' => 'checkbox',
      '#title' => t('Access images via local file access'),
      '#default_value' => variable_get('print_pdf_images_via_file', PRINT_PDF_IMAGES_VIA_FILE_DEFAULT),
      '#description' => t("Enabling this option will make the tool use local file access for image files. This option is not recommended to use in conjunction with modules like imagecache which generate the image after it's first accessed. However, it may be necessary in low-end hosting services where the web server is not allowed to open URLs and the user can't modify that configuration setting."),
    );
    $form['settings']['print_pdf_autoconfig'] = array(
      '#type' => 'checkbox',
      '#title' => t('Auto-configure the PDF tool settings'),
      '#default_value' => variable_get('print_pdf_autoconfig', PRINT_PDF_AUTOCONFIG_DEFAULT),
      '#description' => t('If you disable this option, the pdf tool settings must be configured manually. For TCDPF, edit the tcpdf/config/tcpdf_config.php file. For dompdf, edit the dompdf/dompdf_config.inc.php file.'),
    );

    $form['settings']['print_pdf_filename'] = array(
      '#type' => 'textfield',
      '#title' => t('PDF filename'),
      '#default_value' => variable_get('print_pdf_filename', PRINT_PDF_FILENAME_DEFAULT),
      '#description' => t("If left empty the generated filename defaults to the node's path. Tokens may be used to build the filename (see following list). The .pdf extension will be appended automatically."),
    );
    if (module_exists('token')) {
      $form['settings']['print_pdf_filename_patterns'] = array(
        '#type' => 'fieldset',
        '#title' => t('Replacement patterns'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['settings']['print_pdf_filename_patterns']['descriptions'] = array(
        '#theme' => 'token_tree',
        '#token_types' => array('node'),
      );
    }

    $form['settings']['print_pdf_display_sys_urllist'] = array(
      '#type' => 'checkbox',
      '#title' => t('Printer-friendly URLs list in system pages'),
      '#default_value' => variable_get('print_pdf_display_sys_urllist', PRINT_TYPE_SYS_URLLIST_DEFAULT),
      '#description' => t('Enabling this option will display a list of printer-friendly destination URLs at the bottom of the page.'),
    );

    $form['settings']['link_text'] = array(
      '#type' => 'fieldset',
      '#title' => t('Custom link text'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['settings']['link_text']['print_pdf_link_text_enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable custom link text'),
      '#default_value' => variable_get('print_pdf_link_text_enabled', PRINT_TYPE_LINK_TEXT_ENABLED_DEFAULT),
    );

    $form['settings']['link_text']['print_pdf_link_text'] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('print_pdf_link_text', $link['text']),
      '#description' => t('Text used in the link to the PDF version.'),
    );

    $form['#validate'][] = '_print_pdf_settings_validate';
  }
  else {
    $form['settings'] = array(
      '#type' => 'markup',
      '#markup' => '<p>' . t("No PDF generation tool found! Please dowload a supported PHP PDF generation tool. Check this module's INSTALL.txt for more details.") . '</p>',
    );
  }

  return system_settings_form($form);
}

/**
 * Form validation handler for print_pdf_settings().
 *
 * @see print_pdf_settings()
 * @ingroup forms
 */
function _print_pdf_settings_validate($form, &$form_state) {
  if (empty($form_state['values']['print_pdf_pdf_tool'])) {
    form_set_error('print_pdf_pdf_tool', t("No PDF tool selected"));
  }
}

/**
 * Form constructor for the TCPDF options settings form.
 *
 * @ingroup forms
 */
function print_pdf_tcpdf_settings() {
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('TCPDF options'),
  );

  $form['settings']['print_pdf_font_family'] = array(
    '#type' => 'textfield',
    '#title' => t('Font family'),
    '#default_value' => variable_get('print_pdf_font_family', PRINT_PDF_TCPDF_FONT_FAMILY_DEFAULT),
    '#size' => 60,
    '#maxlength' => 250,
    '#description' => t('Set the font family to be used. Examples: %examples.', array('%examples' => 'helvetica, times, courier, dejavusans, dejavuserif, freesans, freeserif, freemono')) . '<br />' .
                      t("CAUTION: TCPDF embeds the complete font in the generated PDF. If you're not using Unicode, then helvetica or times are safe choices that will keep the PDF small. Unicode fonts can increase the size of the PDF to the 1MB region."),
  );
  $form['settings']['print_pdf_font_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Font size'),
    '#default_value' => variable_get('print_pdf_font_size', PRINT_PDF_TCPDF_FONT_SIZE_DEFAULT),
    '#size' => 2,
    '#maxlength' => 3,
    '#description' => t('Set the font size to be used for normal text. This is the base value for the scaling applied to other text styles.'),
  );
  $form['settings']['print_pdf_font_subsetting'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable font subsetting'),
    '#default_value' => variable_get('print_pdf_font_subsetting', PRINT_PDF_TCPDF_FONT_SUBSETTING_DEFAULT),
    '#description' => t('Only embed those font characters that are actually used.  This can generates smaller PDF files but may significantly slow down processing.'),
  );

  $form['#validate'][] = '_print_pdf_tcpdf_settings_validate';

  return system_settings_form($form);
}

/**
 * Form validation handler for print_pdf_tcpdf_settings().
 *
 * @see print_pdf_tcpdf_settings()
 * @ingroup forms
 */
function _print_pdf_tcpdf_settings_validate($form, &$form_state) {
  if ($form_state['values']['print_pdf_font_size'] < 1) {
    form_set_error('print_pdf_font_size', t("Font size must be at least 1."));
  }
}

/**
 * Form constructor for the dompdf options settings form.
 *
 * @ingroup forms
 */
function print_pdf_dompdf_settings() {
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('dompdf options'),
  );

  $form['settings']['print_pdf_dompdf_unicode'] = array(
    '#type' => 'checkbox',
    '#title' => t("Use dompdf's Unicode Mode"),
    '#default_value' => variable_get('print_pdf_dompdf_unicode', PRINT_PDF_DOMPDF_UNICODE_DEFAULT),
    '#description' => t("If enabled, dompdf's Unicode mode is used. If not, the module will attempt to convert some non-ASCII chars to ISO-8859-1."),
  );

  return system_settings_form($form);
}

/**
 * Form constructor for the wkhtmltopdf options settings form.
 *
 * @ingroup forms
 */
function print_pdf_wkhtmltopdf_settings() {
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('wkhtmltopdf options'),
  );

  $form['settings']['print_pdf_wkhtmltopdf_options'] = array(
    '#type' => 'textfield',
    '#title' => t('wkhtmltopdf options'),
    '#size' => 60,
    '#maxlength' => 500,
    '#default_value' => variable_get('print_pdf_wkhtmltopdf_options', PRINT_PDF_WKHTMLTOPDF_OPTIONS),
    '#description' => t('(wkhtmltopdf only) Set any additional options to be passed to the wkhtmltopdf executable. Tokens may be used in these options (see list below).'),
  );

  return system_settings_form($form);
}

/**
 * Auxiliary function to locate suitable PDF generation tools
 *
 * @return array
 *   array of filenames with the include-able PHP file of the located tools
 */
function _print_pdf_tools() {
  $pattern = '!^(?:dompdf_config.inc.php|tcpdf.php|wkhtmltopdf.*)$!';
  $tools = array_keys(file_scan_directory(drupal_get_path('module', 'print'), $pattern));
  $tools = array_merge($tools, array_keys(file_scan_directory(PRINT_PDF_LIB_PATH, $pattern)));
  if (module_exists('libraries')) {
    $tools = array_merge($tools, array_keys(file_scan_directory(libraries_get_path('dompdf'), '/^dompdf_config.inc.php$/')));
    $tools = array_merge($tools, array_keys(file_scan_directory(libraries_get_path('tcpdf'), '/^tcpdf.php$/')));
    $tools = array_merge($tools, array_keys(file_scan_directory(libraries_get_path('wkhtmltopdf'), '/^wkhtmltopdf/')));
  }


  $num_tools = count($tools);

  if ($num_tools == 0) {
    variable_set('print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT);
    return -1;
  }
  else {
    return array_combine($tools, $tools);
  }
}
