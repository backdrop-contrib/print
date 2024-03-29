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
 * @ingroup forms
 */
function print_pdf_settings() {
  $config = config('print_pdf.settings');
  $pdf_tools = array();
  backdrop_alter('print_pdf_available_libs', $pdf_tools);

  if (!empty($pdf_tools)) {
    $link = print_pdf_print_link();

    $current_pdf_tool = $config->get('print_pdf_pdf_tool');
    $pdf_tool_default = array_key_exists((string) $current_pdf_tool, $pdf_tools) ? $current_pdf_tool : PRINT_PDF_PDF_TOOL_DEFAULT;

    $form['#config'] = 'print_pdf.settings';
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
      '#options' => array(
        t('Same browser window'),
        t('New browser window'),
        t('Save dialog'),
      ),
      '#default_value' => $config->get('print_pdf_content_disposition'),
      '#description' => t("Select the desired method for opening the PDF in the user's browser."),
    );

    $form['settings']['print_pdf_paper_size'] = array(
      '#type' => 'select',
      '#title' => t('Paper size'),
      '#options' => _print_pdf_paper_sizes(),
      '#default_value' => $config->get('print_pdf_paper_size'),
      '#description' => t('Choose the paper size of the generated PDF.'),
    );

    $form['settings']['print_pdf_page_orientation'] = array(
      '#type' => 'select',
      '#title' => t('Page orientation'),
      '#options' => array(
        'portrait' => t('Portrait'),
        'landscape' => t('Landscape'),
      ),
      '#default_value' => $config->get('print_pdf_page_orientation'),
      '#description' => t('Choose the page orientation of the generated PDF.'),
    );

    $form['settings']['print_pdf_cache_enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable caching of generated PDFs for anonymous users'),
      '#default_value' => $config->get('print_pdf_cache_enabled'),
    );

    $form['settings']['print_pdf_cache_lifetime'] = array(
      '#type' => 'select',
      '#title' => t('Cache Lifetime'),
      '#options' => array(
        '0' => 'None',
        '10800' => '3 hours',
        '21600' => '6 hours',
        '43200' => '12 hours',
        '64800' => '18 hours',
        '86400' => '24 hours',
        '129600' => '36 hours',
        '172800' => '2 days',
        '259200' => '3 days',
        '345600' => '4 days',
        '432000' => '5 days',
        '518400' => '6 days',
        '604800' => '7 days',
      ),
      '#default_value' => $config->get('print_pdf_cache_lifetime'),
      '#description' => t('The lifetime of cached PDFs. A cached PDF is only removed when a node is updated, deleted, or cron is run and the last access is older than this value.'),
    );

    $form['settings']['print_pdf_images_via_file'] = array(
      '#type' => 'checkbox',
      '#title' => t('Access images via local file access'),
      '#default_value' => $config->get('print_pdf_images_via_file'),
      '#description' => t("Enabling this option will make the tool use local file access for image files. This option is not recommended to use in conjunction with modules like imagecache which generate the image after it's first accessed. However, it may be necessary in low-end hosting services where the web server is not allowed to open URLs and the user can't modify that configuration setting."),
    );
    $form['settings']['print_pdf_autoconfig'] = array(
      '#type' => 'checkbox',
      '#title' => t('Auto-configure the PDF tool settings'),
      '#default_value' => $config->get('print_pdf_autoconfig'),
      '#description' => t('If you disable this option, the pdf tool settings must be configured manually. For TCDPF, edit the tcpdf/config/tcpdf_config.php file. For dompdf, edit the dompdf/dompdf_config.inc.php file.'),
    );

    $form['settings']['print_pdf_filename'] = array(
      '#type' => 'textfield',
      '#title' => t('PDF filename'),
      '#default_value' => $config->get('print_pdf_filename'),
      '#description' => t("If left empty the generated filename defaults to the node's path. Tokens may be used to build the filename (see following list). The .pdf extension will be appended automatically."),
    );
    if (module_exists('token')) {
      $form['settings']['print_pdf_filename_patterns'] = array(
        '#theme' => 'token_tree',
        '#token_types' => array('node'),
        '#dialog' => TRUE,
      );
    }

    $form['settings']['print_pdf_display_sys_urllist'] = array(
      '#type' => 'checkbox',
      '#title' => t('Printer-friendly URLs list in system pages'),
      '#default_value' => $config->get('print_pdf_display_sys_urllist'),
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
      '#default_value' => $config->get('print_pdf_link_text_enabled'),
    );
    $form['settings']['link_text']['print_pdf_link_text'] = array(
      '#type' => 'textfield',
      '#default_value' => $config->get('print_pdf_link_text'),
      '#description' => t('Text used in the link to the PDF version.'),
    );

    $form['#validate'][] = '_print_pdf_settings_validate';
  }
  else {
    config_set('print_pdf.settings', 'print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT);

    $form['settings'] = array(
      '#type' => 'markup',
      '#markup' => '<p>' . t("No PDF generation tool found! Please download a supported PHP PDF generation tool. Check this module's INSTALL.txt for more details.") . '</p>',
    );
  }

  return system_settings_form($form);
}

/**
 * Form validation handler for print_pdf_settings().
 *
 * @param array $form
 *   Form.
 * @param array $form_state
 *   Form state.
 *
 * @see print_pdf_settings()
 * @ingroup forms
 */
function _print_pdf_settings_validate($form, &$form_state) {
  if (empty($form_state['values']['print_pdf_pdf_tool'])) {
    form_set_error('print_pdf_pdf_tool', t("No PDF tool selected"));
  }
}
