<?php

/**
 * @file
 * Contains the administrative functions of the print_pdf_wkhtmltopdf module.
 *
 * This file is included by the print_pdf_wkhtmltopdf module, and includes the
 * settings form.
 *
 * @ingroup print
 */

/**
 * Form constructor for the wkhtmltopdf options settings form.
 *
 * @ingroup forms
 */
function print_pdf_wkhtmltopdf_settings() {
  $config = config('print_pdf_wkhtmltopdf.settings');
  $form['#config'] = 'print_pdf_wkhtmltopdf.settings';
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('wkhtmltopdf options'),
  );

  $form['settings']['print_pdf_wkhtmltopdf_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to wkhtmltopdf'),
    '#size' => 60,
    '#maxlength' => 500,
    '#default_value' => $config->get('print_pdf_wkhtmltopdf_path'),
    '#description' => t('Enter the path to the wkhtmltopdf executable.'),
  );

  $form['settings']['print_pdf_wkhtmltopdf_options'] = array(
    '#type' => 'textfield',
    '#title' => t('wkhtmltopdf options'),
    '#size' => 60,
    '#maxlength' => 500,
    '#default_value' => $config->get('print_pdf_wkhtmltopdf_options'),
    '#description' => t('Set any additional options to be passed to the wkhtmltopdf executable. Tokens may be used in these options (see list below).'),
  );

  $form['settings']['print_pdf_wkhtmltopdf_use_input_file'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use temporary input file'),
    '#default_value' => $config->get('print_pdf_wkhtmltopdf_use_input_file'),
    '#description' => t('If you experience trouble when generating a PDF try to enable this feature to adjust the input handling.'),
  );

  $form['settings']['print_pdf_filename_patterns'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('node'),
    '#dialog' => TRUE,
  );

  return system_settings_form($form);
}
