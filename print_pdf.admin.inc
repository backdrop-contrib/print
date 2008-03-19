<?php
/* $Id$ */

//*******************************************************************
// Admin Settings: Print
//*******************************************************************

/**
 * Print module settings form
 */
function print_pdf_settings() {
  $print_pdf_settings = variable_get('print_pdf_settings', print_pdf_settings_default());

  $pdf_tools = print_pdf_tools();

  if ($pdf_tools != -1) {
    $form['print_pdf_settings'] = array(
        '#type' => 'fieldset',
        '#tree' => TRUE,
      );

    $form['print_pdf_settings']['show_link'] = array(
      '#type' => 'radios',
      '#title' => t('PDF version link'),
      '#default_value' => $print_pdf_settings['show_link'],
      '#options' => array(t("None (Disabled)"), t("Text only"), t("Icon only"), t("Icon and Text")),
      '#description' => t("Enable or disable the PDF version link for each node. Even if the link is disabled, you can still view the PDF version of a node by going to printpdf/nid where nid is the numeric id of the node."),
    );

    $form['print_pdf_settings']['show_sys_link'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show link in system (non-content) pages'),
      '#return_value' => 1,
      '#default_value' => $print_pdf_settings['show_sys_link'],
      '#description' => t('Setting this option will add a PDF version page link on pages created by Drupal or the enabled modules.'),
    );

    $form['print_pdf_settings']['book_link'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show link in book nodes'),
      '#return_value' => 1,
      '#default_value' => $print_pdf_settings['book_link'],
      '#description' => t('Activate this to add a PDF version link in book nodes. Requires the (core) book module.'),
    );

    $form['print_pdf_settings']['pdf_tool'] = $pdf_tools;
  }
  else {
    $form['print_pdf_settings'] = array(
      '#type' => 'markup',
      '#value' => '<p>'. t("No PDF generation tool found! Please dowload a supported PHP PDF generation tool. Check this module's INSTALL.txt for more details.") .'</p>',
    );
  }

  return system_settings_form($form);
}

/**
 * Print module settings form
 */
function print_pdf_tools() {
  $print_pdf_settings = variable_get('print_pdf_settings', print_pdf_settings_default());

  $tools = array_keys(file_scan_directory(drupal_get_path('module', 'print'), '^dompdf_config.inc.php$'));
  $tools = array_merge($tools, array_keys(file_scan_directory(drupal_get_path('module', 'print'), '^tcpdf.php$')));

  $num_tools = count($tools);

  if ($num_tools == 0) {
    $print_pdf_settings['show_link'] = 0;
    variable_set('print_pdf_settings', $print_pdf_settings);
    return -1;
  }
  else {
    $tools = array_combine($tools, $tools);
    return array(
      '#type' => 'radios',
      '#title' => t('PDF generation tool'),
      '#options' => $tools,
      '#default_value' => $print_pdf_settings['pdf_tool'],
      '#description' => t('This option selects the PDF generation tool being used by this module to create the PDF version.'),
      '#required' => TRUE,
    );
  }
}
