<?php

/**
 * @file
 * Contains the functions to generate Printer-friendly pages.
 *
 * This file is included by the core PF module, and includes all the
 * functions necessary to generate a PF version of the original page
 * in HTML format.
 *
 * @ingroup print
 */

$_print_urls = config_get('print.settings', 'print_urls');

/**
 * Generate an HTML version of the printer-friendly page.
 *
 * @see print_controller()
 */
function print_controller_html() {
  $args = func_get_args();
  $path = filter_xss(implode('/', $args));
  $cid = isset($_GET['comment']) ? (int) $_GET['comment'] : NULL;
  $link = print_print_link();

  $node = print_controller($path, $link['format'], $cid);
  if ($node) {
    // Handle the query.
    $query = $_GET;
    unset($query['q']);

    $html = theme('print', array(
      'node' => $node,
      'query' => $query,
      'format' => $link['format'],
    ));
    backdrop_add_http_header('Content-Type', 'text/html; charset=utf-8');
    backdrop_send_headers();
    print $html;

    $nodepath = (isset($node->nid)) ? 'node/' . $node->nid : backdrop_get_normal_path($path);
    db_merge('print_page_counter')
      ->key(array('path' => substr($nodepath, 0, 255)))
      ->fields(array(
      'totalcount' => 1,
      'timestamp' => REQUEST_TIME,
    ))
      ->expression('totalcount', 'totalcount + 1')
      ->execute();
  }
  else {
    backdrop_exit();
  }
}

/**
 * Select the print generator function based on the page type.
 *
 * Depending on the type of node, this functions chooses the appropriate
 * generator function.
 *
 * @param string $path
 *   Path of the original page.
 * @param string $format
 *   Format of the page being generated.
 * @param int $cid
 *   Comment ID of the individual comment to be rendered.
 * @param string $view_mode
 *   (Optional) view mode to be used when rendering the content.
 *
 * @return object
 *   node-like object to be used in the print template
 *
 * @see _print_generate_node()
 * @see _print_generate_path()
 * @see _print_generate_book()
 * @see print_preprocess_print()
 */
function print_controller($path, $format, $cid = NULL, $view_mode = PRINT_VIEW_MODE) {
  $config = config('print.settings');
  if (empty($path)) {
    // If no path was provided, let's try to generate a page for the referer.
    global $base_url;

    $ref = $_SERVER['HTTP_REFERER'];
    $path = preg_replace("!^$base_url/!", '', $ref);
    if (($path === $ref) || empty($path)) {
      $path = config_get('system.core', 'site_frontpage');
    }
  }
  if ($alias = backdrop_lookup_path('source', $path)) {
    // Indirect call with print/alias
    // If there is a path alias with these arguments, generate a
    // printer-friendly version for it.
    $path = $alias;
  }
  $parts = explode('/', $path);
  $node_router = $config->get('print_node_router');
  if (($parts[0] == 'node') && (count($parts) > 1) && ctype_digit($parts[1]) && !$node_router) {
    array_shift($parts);
    $path = implode('/', $parts);
  }
  $revision_view = preg_match('!^[\d]*/revisions/[\d]*/view$!', $path);
  if (ctype_digit($parts[0]) && ((count($parts) == 1) || $revision_view) && !$node_router) {
    $vid = $revision_view ? $parts[2] : NULL;
    $node = _print_generate_node($path, $format, $vid, $cid, $view_mode);
  }
  else {
    $ret = preg_match('!^book/export/html/(.*)!i', $path, $matches);
    if ($ret == 1) {
      // This is a book PF page link, handle trough the book handling functions.
      $node = _print_generate_book($matches[1], $format);
    }
    else {
      // If no content node was found, handle the page printing with the
      // 'printable' engine.
      $node = _print_generate_path($path, $format);
    }
  }

  return $node;
}

/**
 * Implements hook_preprocess_HOOK().
 */
function print_preprocess_print(&$variables) {
  $config = config('print.settings');
  $node = $variables['node'];
  $format = $variables['format'];
  $path = backdrop_get_path_alias(empty($node->nid) ? $node->path : "node/$node->nid");

  static $hooks = NULL;
  if (!isset($hooks)) {
    backdrop_theme_initialize();
    $hooks = theme_get_registry();
  }

  $variables['page']['#show_messages'] = FALSE;
  $variables['theme_hook_suggestions'] = array();

  // Stolen from theme() so that ALL preprocess functions are called.
  $hook = 'page';
  $info = $hooks[$hook];
  if (isset($info['preprocess functions']) || isset($info['process functions'])) {
    foreach (array('preprocess functions', 'process functions') as $phase) {
      if (!empty($info[$phase])) {
        foreach ($info[$phase] as $processor_function) {
          if (function_exists($processor_function)) {
            // We don't want a poorly behaved process function changing $hook.
            $hook_clone = $hook;
            $processor_function($variables, $hook_clone);
          }
        }
      }
    }
  }

  $logo_url = FALSE;
  switch ($config->get('print_logo_options')) {
    // Theme logo.
    case 1:
      $logo_url = theme_get_setting('logo');
      break;

      // User-specifed logo.
    case 2:
      $logo_url = strip_tags($config->get('print_logo_url'));
      break;
  }
  $logo_url = preg_replace('!^' . base_path() . '!', '', $logo_url);

  $variables['print_logo'] = $logo_url ? theme('image', array(
    'path' => $logo_url,
    'alt' => config_get('system.core', 'site_name'),
    'attributes' => array(
      'class' => array('print-logo'),
      'id' => 'logo',
    ),
  )) : NULL;

  $variables['print_node'] = $node;
  $variables['content'] = $node->content;
  $variables['scripts'] = backdrop_get_js();
  $variables['footer_scripts'] = backdrop_get_js('footer');
  $variables['sourceurl_enabled'] = $config->get('print_sourceurl_enabled');
  $variables['url'] = url($path, array('absolute' => TRUE, 'query' => $variables['query']));
  $variables['source_url'] = url($config->get('print_sourceurl_forcenode') ? backdrop_get_normal_path($path) : $path, array(
    'alias' => TRUE,
    'absolute' => TRUE,
    'query' => $variables['query'],
  ));
  $variables['cid'] = isset($node->cid) ? $node->cid : NULL;
  $variables['print_title'] = html_entity_decode(check_plain($node->title));
  $variables['head'] = backdrop_get_html_head();
  $variables['robots_meta'] = _print_robots_meta_generator();
  $variables['css'] = _print_css_generator($variables['expand_css']);

  if ($config->get('print_html_sendtoprinter') && ($format == 'html')) {
    backdrop_add_js('core/misc/backdrop.js', array('group' => JS_LIBRARY));

    $window_close = ($config->get('print_html_new_window') && $config->get('print_html_windowclose')) ? 'setTimeout(function(){window.close();}, 1);' : '';
    $variables['sendtoprinter'] = '<script type="text/javascript">(function ($) { Backdrop.behaviors.print = {attach: function() {$(window).load(function() {window.print();' . $window_close . '})}}})(jQuery);</script>';
  }

  $type = (isset($node->type)) ? $node->type : '';
  $nid = (isset($node->nid)) ? $node->nid : '';

  $variables['theme_hook_suggestions'][] = "print__node__{$type}";
  $variables['theme_hook_suggestions'][] = "print__node__{$type}__{$nid}";
  $variables['theme_hook_suggestions'][] = "print__{$format}";
  $variables['theme_hook_suggestions'][] = "print__{$format}__node__{$type}";
  $variables['theme_hook_suggestions'][] = "print__{$format}__node__{$type}__{$nid}";
}

/**
 * Returns HTML for the published line of the print template.
 *
 * @param array $vars
 *   An empty associative array.
 *
 * @return string
 *   HTML text with the published line
 *
 * @ingroup themeable
 * @ingroup print_themeable
 */
function theme_print_published($vars) {
  global $base_url;

  $published_site = config_get('system.core', 'site_name');
  return $published_site ? t('Published on %site_name', array('%site_name' => $published_site)) . ' (' . l($base_url, $base_url) . ')' : '';
}

/**
 * Returns HTML for the breadcrumb line of the print template.
 *
 * @param array $vars
 *   An associative array containing:
 *   - $node: the node object.
 *
 * @return string
 *   HTML text with the breadcrumb
 *
 * @ingroup themeable
 * @ingroup print_themeable
 */
function theme_print_breadcrumb($vars) {
  $node = $vars['node'];
  $old_path = $_GET['q'];

  $path = empty($node->nid) ? $node->path : "node/$node->nid";
  menu_set_active_item($path);
  $breadcrumb = backdrop_get_breadcrumb();
  if (!empty($breadcrumb)) {
    $breadcrumb[] = menu_get_active_title();
    menu_set_active_item($old_path);
    return filter_xss(implode(' > ', $breadcrumb));
  }
  else {
    menu_set_active_item($old_path);
    return '';
  }
}

/**
 * Returns HTML for the footer of the print template.
 *
 * @param array $vars
 *   An empty associative array.
 *
 * @return string
 *   HTML text with the footer
 *
 * @ingroup themeable
 * @ingroup print_themeable
 */
function theme_print_footer($vars) {
  $config = config('print.settings');
  $footer = '';

  switch ($config->get('print_footer_options')) {
    // Theme footer.
    case 1:
      $footer_blocks = block_get_blocks_by_region('footer');
      $footer = backdrop_render($footer_blocks);
      break;

      // User-specified footer.
    case 2:
      $footer = $config->get('print_footer_user');
      break;
  }
  // Delete the contextual links.
  $footer = preg_replace('!\s*<div class="contextual-links-wrapper">.*?</div>!sim', '', $footer);

  return filter_xss_admin($footer);
}

/**
 * Returns HTML for the source URL line of the print template.
 *
 * @param array $vars
 *   An associative array containing:
 *   - $url: the URL to the full node view.
 *   - $node: the node object.
 *   - $cid; comment ID of the comment to display.
 *
 * @return string
 *   HTML text with the footer
 *
 * @ingroup themeable
 * @ingroup print_themeable
 */
function theme_print_sourceurl($vars) {
  global $base_url;
  $sourceurl_date = config_get('print.settings', 'print_sourceurl_date');
  $url = is_int($vars['cid']) ? $vars['url'] . '#comment-' . $vars['cid'] : $vars['url'];

  $output = '<strong>' . t('Source URL');
  if ($sourceurl_date && isset($vars['node'])) {
    $output .= ' (';
    $date = format_date($vars['node']->changed, 'short');

    $output .= empty($vars['node']->nid) ? t('retrieved on !date', array('!date' => $date)) : t('modified on !date', array('!date' => $date));

    $output .= ')';
  }
  $new_url = explode($base_url, $url);
  $output .= ':</strong>' . "<a href=" . $new_url[1] . ">" . $url . '</a>';

  return $output;
}

/**
 * Returns HTML for the URL list of the print template.
 *
 * @param array $vars
 *   An empty associative array.
 *
 * @return string
 *   HTML text with the URL list
 *
 * @ingroup themeable
 * @ingroup print_themeable
 */
function theme_print_url_list($vars) {
  global $_print_urls;
  global $base_url;

  // Display the collected links at the bottom of the page. Code once taken from
  // Kjartan Mannes' project.module.
  if ($_print_urls) {
    $urls = _print_friendly_urls();
    $url_list = '';
    foreach ($urls as $key => $url) {
      backdrop_alter('print_url_list', $url);
      $new_url = explode($base_url, $url);
      $url_list .= "<a href=" . $new_url[1] . " target='_blank'>" . '[' . ($key + 1) . '] ' . check_plain($url) . "</a>\n";
    }
    if (!empty($url_list)) {
      return "<p><strong>" . t('Links') . "</strong><br />$url_list</p>";
    }
  }
  return '';
}

/**
 * Generates a robots meta tag to tell them what they may index.
 *
 * @return string
 *   meta robots tag
 */
function _print_robots_meta_generator() {
  $config = config('print.settings');
  $robots_meta = array();

  if ($config->get('print_robots_noindex')) {
    $robots_meta[] = 'noindex';
  }
  if ($config->get('print_robots_nofollow')) {
    $robots_meta[] = 'nofollow';
  }
  if ($config->get('print_robots_noarchive')) {
    $robots_meta[] = 'noarchive';
  }

  if (count($robots_meta) > 0) {
    return '<meta name="robots" content="' . implode(', ', $robots_meta) . '" />';
  }
  else {
    return '';
  }
}

/**
 * Generates the CSS directive to include in the printer-friendly version.
 *
 * @param bool $expand
 *   If TRUE, the provided CSS will be expanded, instead of given as a list
 *   of includes.
 *
 * @return string
 *   applicable CSS
 */
function _print_css_generator($expand = FALSE) {
  $config = config('print.settings');
  $print_css = $config->get('print_css');

  if (!empty($print_css)) {
    backdrop_add_css(strtr($print_css, array('%t' => backdrop_get_path('theme', config_get('system.core', 'theme_default')))));
  }
  else {
    backdrop_add_css(backdrop_get_path('module', 'print') . '/css/print.css');
  }
  $backdrop_css = backdrop_add_css();
  if (!$config->get('print_keep_theme_css')) {
    foreach ($backdrop_css as $key => $css_file) {
      if ($css_file['group'] == CSS_THEME) {
        // Unset the theme's CSS.
        unset($backdrop_css[$key]);
      }
    }
  }

  // Expand the CSS if requested.
  if ($expand) {
    $style = '';
    $css_files = array_keys($backdrop_css);
    foreach ($css_files as $filename) {
      if (file_exists($filename)) {
        $style .= file_get_contents($filename, TRUE);
      }
    }
    return "<style type='text/css' media='all'>$style</style>\n";
  }
  else {
    return backdrop_get_css($backdrop_css);
  }
}

/**
 * Callback function for the preg_replace_callback for URL-capable patterns.
 *
 * Manipulate URLs to make them absolute in the URLs list, and add a [n]
 * footnote marker.
 *
 * @param array $matches
 *   Array with the matched tag patterns, usually <a...>+text+</a>.
 *
 * @return string
 *   tag with re-written URL and, if applicable, the [n] index to the URL list
 */
function _print_rewrite_urls($matches) {
  global $base_url, $base_root, $_print_urls;

  $include_anchors = config_get('print.settings', 'print_urls_anchors');

  // First, split the html into the different tag attributes.
  $pattern = '!\s*(\w+\s*=\s*"(?:\\\"|[^"])*")\s*|\s*(\w+\s*=\s*\'(?:\\\\\'|[^\'])*\')\s*|\s*(\w+\s*=\s*\w+)\s*|\s+!';
  $attribs = preg_split($pattern, $matches[1], -1, PREG_SPLIT_NO_EMPTY | PREG_SPLIT_DELIM_CAPTURE);
  foreach ($attribs as $key => $value) {
    $attribs[$key] = preg_replace('!(\w)\s*=\s*(.*)!', '$1=$2', $value);
  }

  $size = count($attribs);
  for ($i = 1; $i < $size; $i++) {
    // If the attribute is href or src, rewrite the URL in the value.
    if (preg_match('!^(?:href|src)\s*?=(.*)!i', $attribs[$i], $urls) > 0) {
      $url = trim($urls[1], " \t\n\r\0\x0B\"'");

      if (empty($url)) {
        // If URL is empty, use current_url.
        $path = explode('/', $_GET['q']);
        unset($path[0]);
        $path = implode('/', $path);
        if (ctype_digit($path)) {
          $path = "node/$path";
        }
        // Printer-friendly URLs is on, so we need to make it absolute.
        $newurl = url($path, array(
          'fragment' => backdrop_substr($url, 1),
          'absolute' => TRUE,
        ));
      }
      elseif (strpos(html_entity_decode($url), '://') || preg_match('!^mailto:.*?@.*?\..*?$!iu', html_entity_decode($url))) {
        // URL is absolute, do nothing.
        $newurl = $url;
      }
      elseif (strpos(html_entity_decode($url), '//') === 0) {
        // URL is 'almost absolute', but it does not contain protocol; replace
        // with base_path protocol.
        $newurl = (empty($_SERVER['HTTPS']) ? 'http' : 'https') . ":" . $url;
        $matches[1] = str_replace($url, $newurl, $matches[1]);
      }
      else {
        if ($url[0] == '#') {
          // URL is an anchor tag.
          if ($include_anchors && ($_print_urls)) {
            $path = explode('/', $_GET['q']);
            unset($path[0]);
            $path = implode('/', $path);
            if (ctype_digit($path)) {
              $path = "node/$path";
            }
            // Printer-friendly URLs is on, so we need to make it absolute.
            $newurl = url($path, array(
              'fragment' => backdrop_substr($url, 1),
              'absolute' => TRUE,
            ));
          }
          // Because base href is the original page, change the link to
          // still be usable inside the print page.
          $matches[1] = str_replace($url, check_plain(base_path() . $_GET['q'] . $url), $matches[1]);
        }
        else {
          // URL is relative, convert it into absolute URL.
          if ($url[0] == '/') {
            // If it starts with '/' just append it to the server name.
            $newurl = $base_root . '/' . trim($url, '/');
          }
          elseif (preg_match('!^(?:index.php)?\?q=!i', $url)) {
            // If it starts with ?q=, just prepend with the base URL.
            $newurl = $base_url . '/' . trim($url, '/');
          }
          else {
            $newurl = url(trim($url, '/'), array('absolute' => TRUE));
          }
          $matches[1] = str_replace($url, $newurl, $matches[1]);
        }
      }
    }
  }

  $ret = '<' . $matches[1] . '>';
  if (count($matches) == 4) {
    $ret .= $matches[2] . $matches[3];
    if (($_print_urls) && (isset($newurl))) {
      $ret .= ' <span class="print-footnote">[' . _print_friendly_urls(trim($newurl)) . ']</span>';
    }
  }

  return filter_xss_admin($ret);
}

/**
 * Auxiliary function to store the Printer-friendly URLs list as static.
 *
 * @param string|int $url
 *   Absolute URL to be inserted in the list.
 *
 * @return array|int
 *   list of URLs previously stored if $url is 0, or the current count
 *   otherwise.
 */
function _print_friendly_urls($url = 0) {
  $urls = &backdrop_static(__FUNCTION__, array());
  if ($url !== 0) {
    $url_idx = array_search($url, $urls);
    if ($url_idx !== FALSE) {
      return ($url_idx + 1);
    }
    else {
      $urls[] = $url;
      return count($urls);
    }
  }
  else {
    $ret = $urls;
    $urls = array();
    return $ret;
  }
}

/**
 * Check URL list settings for this node.
 *
 * @param Object $node
 *   Node object.
 * @param string $format
 *   Format of the page being generated.
 *
 * @return bool
 *   TRUE if URL list should be displayed, FALSE otherwise
 */
function _print_url_list_enabled($node, $format) {
  $config = config('print.settings');
  if (!isset($node->type)) {
    $node_urllist = $config->get('print_' . $format . '_display_sys_urllist');
  }
  else {
    $node_urllist = isset($node->{'print_' . $format . '_display_urllist'}) ? $node->{'print_' . $format . '_display_urllist'} : $config->get('print_' . $format . '_display_urllist_' . $node->type);
  }

  // Get value of Printer-friendly URLs setting.
  return ($config->get('print_urls') && ($node_urllist));
}

/**
 * Prepare a Printer-friendly-ready node body for content nodes.
 *
 * @param int $nid
 *   Node ID of the node to be rendered into a printer-friendly page.
 * @param string $format
 *   Format of the page being generated.
 * @param int $vid
 *   (Optional) revision ID of the node to use.
 * @param int $cid
 *   (Optional) comment ID of the individual comment to be rendered.
 * @param string $view_mode
 *   (Optional) view mode to be used when rendering the content.
 *
 * @return object|bool
 *   filled node-like object to be used in the print template
 */
function _print_generate_node($nid, $format, $vid = NULL, $cid = NULL, $view_mode = PRINT_VIEW_MODE) {
  global $_print_urls;

  if (!isset($langcode)) {
    $langcode = $GLOBALS['language_content']->langcode;
  }

  // We can take a node id.
  $node = node_load($nid, $vid);
  if (!$node) {
    // Node not found.
    backdrop_not_found();
    backdrop_exit();
  }
  elseif (!node_access('view', $node)) {
    // Access is denied.
    backdrop_access_denied();
    backdrop_exit();
  }
  backdrop_set_title($node->title);

  $build = array();
  if ($cid === NULL) {
    // Adapted (simplified) version of node_view
    // Render the node content.
    node_build_content($node, $view_mode);

    // Disable the links area.
    unset($node->content['links']);

    $build = $node->content;
    unset($node->content);
  }

  if (function_exists('comment_node_page_additions') &&
      (($cid != NULL) || (config_get('print.settings', 'print_comments')))) {
    // Print only the requested comment (or if $cid is NULL, all of them).
    $comments = comment_node_page_additions($node);
    if (!empty($comments)) {
      unset($comments['comment_form']);
      foreach ($comments['comments'] as $key => &$comment) {
        if (is_numeric($key)) {
          if (($cid != NULL) && ($key != $cid)) {
            unset($comments['comments'][$key]);
          }
          else {
            unset($comment['links']);
          }
        }
      }

      $build['comments'] = $comments;
    }
  }

  $build += array(
    '#theme' => 'node',
    '#node' => $node,
    '#view_mode' => $view_mode,
    '#langcode' => $langcode,
    '#print_format' => $format,
  );

  $type = 'node';
  backdrop_alter(array('node_view', 'entity_view'), $build, $type);

  $content = render($build);

  // Get rid of any links before the content.
  $parts = explode('<div class="content', $content, 2);
  if (count($parts) == 2) {
    $pattern = '!(.*?)<a [^>]*?>(.*?)</a>(.*?)!mis';
    $parts[0] = preg_replace($pattern, '$1$2$3', $parts[0]);
    $content = implode('<div class="content', $parts);
  }

  // Check URL list settings.
  $_print_urls = _print_url_list_enabled($node, $format);

  // Convert the a href elements.
  $pattern = '!<(a\s[^>]*?)>(.*?)(</a>)!is';
  $content = preg_replace_callback($pattern, '_print_rewrite_urls', $content);

  $node->content = $content;

  return $node;
}

/**
 * Prepare a Printer-friendly-ready node body for non-content pages.
 *
 * @param string $path
 *   Path of the node to be rendered into a printer-friendly page.
 * @param string $format
 *   Format of the page being generated.
 *
 * @return object|bool
 *   filled node-like object to be used in the print template
 */
function _print_generate_path($path, $format) {
  global $_print_urls;

  // Handle node tabs, or cases where the 'node_router' option is enabled.
  $parts = explode('/', $path);
  if (ctype_digit($parts[0]) && ((count($parts) > 1) || config_get('print.settings', 'print_node_router'))) {
    $path = 'node/' . $path;
  }

  $path = backdrop_get_normal_path($path);

  menu_set_active_item($path);
  // Adapted from index.php.
  $node = new stdClass();
  $node->content = menu_execute_active_handler($path, FALSE);
  if (is_array($node->content)) {
    $node->content = backdrop_render($node->content);
  }

  if (is_int($node->content)) {
    switch ($node->content) {
      case MENU_NOT_FOUND:
        backdrop_not_found();
        backdrop_exit();
        break;

      case MENU_ACCESS_DENIED:
        backdrop_access_denied();
        backdrop_exit();
        break;
    }
  }

  $node->title = backdrop_get_title();
  $node->path = $path;
  $node->changed = REQUEST_TIME;
  $node->type = '';

  // Delete any links area.
  $node->content = preg_replace('!\s*<div class="links">.*?</div>!sim', '', $node->content);

  // Delete the contextual links also.
  $node->content = preg_replace('!\s*<div class="contextual-links-wrapper">.*?</div>!sim', '', $node->content);

  // Check URL list settings.
  $_print_urls = _print_url_list_enabled($node, $format);

  // Convert the a href elements.
  $pattern = '!<(a\s[^>]*?)>(.*?)(</a>)!is';
  $node->content = preg_replace_callback($pattern, '_print_rewrite_urls', $node->content);

  return $node;
}

/**
 * Prepare a Printer-friendly-ready node body for book pages.
 *
 * @param int $nid
 *   Node ID of the node to be rendered into a printer-friendly page.
 * @param string $format
 *   Format of the page being generated.
 *
 * @return object|bool
 *   filled node-like object to be used in the print template
 */
function _print_generate_book($nid, $format) {
  global $_print_urls;

  $node = node_load($nid);
  if (!$node) {
    // Node not found.
    backdrop_not_found();
    backdrop_exit();
  }
  elseif (!node_access('view', $node) || (!user_access('access printer-friendly version'))) {
    // Access is denied.
    backdrop_access_denied();
    backdrop_exit();
  }

  $tree = book_menu_subtree_data($node->book);
  $node->content = book_export_traverse($tree, 'book_node_export');

  // Check URL list settings.
  $_print_urls = _print_url_list_enabled($node, $format);

  // Convert the a href elements.
  $pattern = '!<(a\s[^>]*?)>(.*?)(</a>)!is';
  $node->content = preg_replace_callback($pattern, '_print_rewrite_urls', $node->content);

  return $node;
}
