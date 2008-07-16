<?php
// $Id$

/**
 * @file
 * Contains the administrative functions of the send by e-mail module.
 *
 * This file is included by the send by e-mail module, and includes the
 * settings form.
 */

/**
 * Menu callback for the send by e-mail module settings form.
 *
 * @ingroup forms
 */
function print_mail_settings() {
  $print_mail_settings = variable_get('print_mail_settings', print_mail_settings_default());

  $form['print_mail_settings'] = array(
      '#type' => 'fieldset',
      '#tree' => TRUE,
    );

  $form['print_mail_settings']['show_link'] = array(
    '#type' => 'radios',
    '#title' => t('Send by e-mail link'),
    '#default_value' => $print_mail_settings['show_link'],
    '#options' => array(t("None (Disabled)"), t("Text only"), t("Icon only"), t("Icon and Text")),
    '#description' => t("Enable or disable the send by e-mail link for each node. Even if the link is disabled, you can still send a node by e-mail by going to printmail/nid where nid is the numeric id of the node."),
  );

  $form['print_mail_settings']['node_link_visibility'] = array(
    '#type' => 'radios',
    '#title' => t('Link visibility'),
    '#default_value' => $print_mail_settings['node_link_visibility'],
    '#options' => array(t('Show on every page except the listed pages.'), t('Show on only the listed pages.')),
  );

  $form['print_mail_settings']['node_link_pages'] = array(
    '#type' => 'textarea',
    '#default_value' => $print_mail_settings['node_link_pages'],
    '#rows' => 3,
    '#description' => t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>')),
  );

  $form['print_mail_settings']['link_class'] = array(
    '#type' => 'textfield',
    '#title' => t('Link class'),
    '#default_value' => $print_mail_settings['link_class'],
    '#size' => 60,
    '#maxlength' => 250,
    '#description' => t('This can be used by themers to change the link style or by jQuery modules to open in a new window (e.g. greybox or thickbox). Multiple classes can be specified, separated by spaces.'),
  );

  $form['print_mail_settings']['sys_link_visibility'] = array(
    '#type' => 'radios',
    '#title' => t('Show link in system (non-content) pages'),
    '#default_value' => $print_mail_settings['sys_link_visibility'],
    '#options' => array(t('Show on every page except the listed pages.'), t('Show on only the listed pages.')),
  );

  $form['print_mail_settings']['sys_link_pages'] = array(
    '#type' => 'textarea',
    '#default_value' => $print_mail_settings['sys_link_pages'],
    '#rows' => 3,
    '#description' => t("Setting this option will add a send by-email link on pages created by Drupal or the enabled modules.") .'<br />'.
                      t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>')),
  );

  $form['print_mail_settings']['book_link'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show link in book nodes'),
    '#default_value' => $print_mail_settings['book_link'],
    '#description' => t('Activate this to add a send by e-mail link in book nodes. Requires the (core) book module.'),
  );

  $form['print_mail_settings']['teaser_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send only the teaser'),
    '#default_value' => $print_mail_settings['teaser_default'],
    '#description' => t("If selected, the default choice will be to send only the node's teaser instead of the full content."),
  );

  $form['print_mail_settings']['teaser_choice'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable teaser/full mode choice'),
    '#default_value' => $print_mail_settings['teaser_choice'],
    '#description' => t('If checked, the user will be able to choose between sending the full content or only the teaser at send time.'),
  );

  return system_settings_form($form);
}
