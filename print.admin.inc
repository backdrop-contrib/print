<?php

/**
 * @file
 * Contains the administrative functions of the PF module.
 *
 * This file is included by the core PF module, and includes the
 * settings form.
 *
 * @ingroup print
 */

/**
 * Form constructor for the Printer-friendly pages module settings form.
 *
 * @ingroup forms
 */
function print_main_settings() {
  $config = config('print.settings');
  $print_css = $config->get('print_css');
  // Check that the stylesheet path is indeed valid and relative to base_path()
  if (!empty($print_css) && empty($_POST)) {
    $css_path = BACKDROP_ROOT . '/' . strtr($print_css, array(
      '%t' => backdrop_get_path('theme', config_get('system.core', 'theme_default')),
    ));
    if (!is_file($css_path) || !is_readable($css_path)) {
      backdrop_set_message(t("File %file is not readable by the web server.", array('%file' => $css_path)), 'warning', FALSE);
    }
  }

  $form['#config'] = 'print.settings';
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Common settings'),
  );

  $form['settings']['print_css'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom stylesheet'),
    '#default_value' => $print_css,
    '#size' => 60,
    '#maxlength' => 250,
    '#description' => t('The path to your custom print cascading stylesheet, if any. The path provided must be relative to the base path. When none is specified, the default module CSS file is used.') . '<br />' .
      t('Macros: %t (path to theme: "%theme")', array(
      '%theme' => backdrop_get_path('theme', config_get('system.core', 'theme_default')),
    )) . '<br />' .
      t('Requires the <em>Administer site configuration</em> permission.'),
    '#disabled' => !user_access('administer site configuration'),
  );

  $form['settings']['print_keep_theme_css'] = array(
    '#type' => 'checkbox',
    '#title' => t('Keep the current theme CSS'),
    '#default_value' => $config->get('print_keep_theme_css'),
    '#description' => t('If set, the current theme css will still be applied to the printer-friendly versions.'),
  );

  $form['settings']['print_urls'] = array(
    '#type' => 'checkbox',
    '#title' => t('Printer-friendly URLs list'),
    '#default_value' => $config->get('print_urls'),
    '#description' => t('If set, a list of the destination URLs for the page links will be displayed at the bottom of the page.') . '<br />' .
      t('Note that you can enable/disable the URL list for each content type individually from the !url.', array(
      '!url' => l(t('Content Types page'), 'admin/structure/types'),
    )),
  );

  $form['settings']['print_urls_anchors'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include named anchors in printer-friendly URLs list'),
    '#default_value' => $config->get('print_urls_anchors'),
    '#description' => t('If set, the list of the URLs at the bottom of the page will include anchors links on the same page.'),
  );

  $form['settings']['print_comments'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include comments in printer-friendly version'),
    '#default_value' => $config->get('print_comments'),
    '#description' => t('When this option is active, user comments are also included in the printer-friendly version. Requires the comment module.'),
  );

  $form['settings']['print_node_router'] = array(
    '#type' => 'checkbox',
    '#title' => t('Consult menu router for node display handler'),
    '#default_value' => $config->get('print_node_router'),
    '#description' => t('When this option is active, print module will check the menu router for the handler to consult for page contents at node/%node.'),
  );

  $form['settings']['print_newwindow'] = array(
    '#type' => 'radios',
    '#title' => t('New window method'),
    '#options' => array(t('Use HTML target (does not validate as XHTML Strict)'), t('Use Javascript (requires browser support)')),
    '#default_value' => $config->get('print_newwindow'),
    '#description' => t('Choose the method used to open pages in a new window/tab.'),
  );

  $form['settings']['logo'] = array(
    '#type' => 'fieldset',
    '#title' => t('Logo options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['settings']['logo']['print_logo_options'] = array(
    '#type' => 'radios',
    '#title' => t('Logo type'),
    '#options' => array(
      t('None (Disabled)'),
      t("Current theme's logo"),
      t('User-specified'),
    ),
    '#default_value' => $config->get('print_logo_options'),
    '#description' => t("Select the type of logo to display on the printer-friendly version. In case of a user-specified location, insert the path or URL below."),
  );

  $form['settings']['logo']['print_logo_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Logo URL'),
    '#default_value' => $config->get('print_logo_url'),
    '#size' => 60,
    '#maxlength' => 250,
  );

  $form['settings']['logo']['print_logo_upload'] = array(
    '#type' => 'file',
    '#title' => t('Upload logo'),
    '#size' => 60,
    '#maxlength' => 250,
    '#description' => t("If you don't have direct file access to the server, use this field to upload your logo."),
  );

  $form['settings']['footer'] = array(
    '#type' => 'fieldset',
    '#title' => t('Footer options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['settings']['footer']['print_footer_options'] = array(
    '#type' => 'radios',
    '#title' => t('Footer type'),
    '#options' => array(
      t('None (Disabled)'),
      t("Site's footer"),
      t('User-specified'),
    ),
    '#default_value' => $config->get('print_footer_options'),
    '#description' => t("Select the type of footer to display on the printer-friendly version. In case of a user-specified footer, insert it below."),
  );

  $form['settings']['footer']['print_footer_user'] = array(
    '#type' => 'textfield',
    '#title' => t('User-specified'),
    '#default_value' => $config->get('print_footer_user'),
    '#size' => 60,
    '#maxlength' => 250,
  );

  $form['settings']['print_sourceurl_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Source URL'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['settings']['print_sourceurl_settings']['print_sourceurl_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display source URL'),
    '#default_value' => $config->get('print_sourceurl_enabled'),
    '#description' => t('When this option is selected, the URL for the original page will be displayed at the bottom of the printer-friendly version.'),
  );

  $form['settings']['print_sourceurl_settings']['print_sourceurl_date'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add current time/date to the source URL'),
    '#default_value' => $config->get('print_sourceurl_date'),
    '#description' => t('Display the current date and time in the Source URL line.'),
  );

  $form['settings']['print_sourceurl_settings']['print_sourceurl_forcenode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force use of node ID in source URL'),
    '#default_value' => $config->get('print_sourceurl_forcenode'),
    '#description' => t("Backdrop will attempt to use the page's defined alias in case there is one. To force the use of the fixed URL, activate this option."),
  );

  $form['#attributes'] = array('enctype' => 'multipart/form-data');
  $form['#validate'][] = '_print_main_settings_validate';

  return system_settings_form($form);
}

/**
 * Form validation handler for print_main_settings().
 */
function _print_main_settings_validate($form, &$form_state) {
  global $base_root;

  // Check for a new uploaded logo, and use that instead.
  if ($file = file_save_upload('print_logo_upload', array('file_validate_is_image' => array()))) {
    // The image was saved using file_save_upload() and was added to the
    // files table as a temporary file. We'll make a copy and let the garbage
    // collector delete the original upload.
    if ($filename = file_unmanaged_copy($file->uri)) {
      /** @var BackdropStreamWrapperInterface $wrapper */
      $wrapper = file_stream_wrapper_get_instance_by_uri($filename);

      $form_state['values']['print_logo_options'] = 2;
      $form_state['values']['print_logo_url'] = str_replace($base_root, '', $wrapper->getExternalUrl());
    }
  }
}

/**
 * Form constructor for the Printer-friendly pages HTML settings form.
 *
 * @ingroup forms
 */
function print_html_settings() {
  $config = config('print.settings');
  $link = print_print_link();

  $form['#config'] = 'print.settings';
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Web page options'),
  );

  $form['settings']['print_html_new_window'] = array(
    '#type' => 'checkbox',
    '#title' => t('Open the printer-friendly version in a new window'),
    '#default_value' => $config->get('print_html_new_window'),
    '#description' => t('Setting this option will make the printer-friendly version open in a new window/tab.'),
  );

  $form['settings']['print_html_sendtoprinter'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send to printer'),
    '#default_value' => $config->get('print_html_sendtoprinter'),
    '#description' => t("Automatically calls the browser's print function when the printer-friendly version is displayed."),
  );

  $form['settings']['print_html_windowclose'] = array(
    '#type' => 'checkbox',
    '#title' => t('Close window after sending to printer'),
    '#default_value' => $config->get('print_html_windowclose'),
    '#description' => t("When the above options are enabled, this option will close the window after its contents are printed."),
  );

  $form['settings']['print_html_display_sys_urllist'] = array(
    '#type' => 'checkbox',
    '#title' => t('Printer-friendly URLs list in system pages'),
    '#default_value' => $config->get('print_html_display_sys_urllist'),
    '#description' => t('Enabling this option will display a list of printer-friendly destination URLs at the bottom of the page.'),
  );

  $form['settings']['print_html_link_text'] = array(
    '#title' => t('Link text'),
    '#type' => 'textfield',
    '#default_value' => (!empty($config->get('print_html_link_text'))) ? $config->get('print_html_link_text') : t('Printer-friendly version'),
    '#description' => t('Text used in the link to the printer-friendly version.'),
  );

  $form['settings']['print_robots_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Robots META tags'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['settings']['print_robots_settings']['print_robots_noindex'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add noindex'),
    '#default_value' => $config->get('print_robots_noindex'),
    '#description' => t('Instruct robots to not index printer-friendly pages. Recommended for good search engine karma.'),
  );

  $form['settings']['print_robots_settings']['print_robots_nofollow'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add nofollow'),
    '#default_value' => $config->get('print_robots_nofollow'),
    '#description' => t('Instruct robots to not follow outgoing links on printer-friendly pages.'),
  );

  $form['settings']['print_robots_settings']['print_robots_noarchive'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add noarchive'),
    '#default_value' => $config->get('print_robots_noarchive'),
    '#description' => t('Non-standard tag to instruct search engines to not show a "Cached" link for your printer-friendly pages. Recognized by Googlebot.'),
  );

  return system_settings_form($form);
}
