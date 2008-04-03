<?php
/* $Id$ */

//*******************************************************************
// Module Functions :: Controller
//*******************************************************************

function print_controller_html() {
  $print = print_controller();
  include_once(_print_get_template($print["type"]));
}

/**
 * Print module path catcher
 */
function print_controller() {
  // Remove the print/ prefix
  $args = substr($_GET['q'], strpos($_GET['q'], '/')+1);

  $cid = isset($_GET['comment']) ? $_GET['comment'] : NULL;

  $nid = $args;
  if (!is_numeric($nid)) {
    // Indirect call with print/alias
    // If there is a path alias with these arguments, generate a printer-friendly version for it
    $path = drupal_get_normal_path($args);
    $ret = preg_match("/^node\/(.*)/i", $path, $matches);
    if ($ret == 1) {
      $nid = $matches[1];
    }
  }
  if (is_numeric($nid)) {
    $print = _print_generate_node($nid, $cid);
  }
  else {
    $ret = preg_match("/^book\/export\/html\/(.*)/i", $args, $matches);
    if ($ret == 1) {
      // This is a book PF page link, handle trough the book handling functions
      $print = _print_generate_book($matches[1]);
    }
    else {
      // If no content node was found, handle the page printing with the 'printable' engine
      $print = _print_generate_path($args);
    }
  }

  return $print;
}

//*******************************************************************
// Module Functions : Auxiliary
//*******************************************************************

/**
 * Generates a meta tag to tell robots what they may index based on module settings
 *
 * @return string
 */
function _print_robots_meta_generator() {
  $robots_settings = variable_get('print_robot_settings', print_robot_settings_default());
  $robots_meta = array();

  if (!empty($robots_settings['noindex'])) {
    $robots_meta[] = 'noindex';
  }
  if (!empty($robots_settings['nofollow'])) {
    $robots_meta[] = 'nofollow';
  }
  if (!empty($robots_settings['noarchive'])) {
    $robots_meta[] = 'noarchive';
  }
  if (!empty($robots_settings['nocache'])) {
    $robots_meta[] = 'nocache';
  }

  if (sizeof($robots_meta) > 0) {
    $robots_meta = isset($robots_meta[1]) ? implode(', ', $robots_meta) : $robots_meta[0];
    $robots_meta = '<meta name="robots" content="'. $robots_meta ."\" />\n";
  }
  else {
    $robots_meta = '';
  }

  return $robots_meta;
}

/**
 * Generates the HTML to insert in the template file
 *
 * @return string
 */
function _print_var_generator($node, $cid = NULL) {
  global $base_url, $language;

  $path = empty($node->nid) ? $node->path : "node/$node->nid";

  $themed = theme('print_text');

  // print module settings
  $print_settings = variable_get('print_settings', print_settings_default());
  $print_html_settings = variable_get('print_html_settings', print_html_settings_default());
  $print_sourceurl_settings = variable_get('print_sourceurl_settings', print_sourceurl_settings_default());

  $print["language"] = $language->language;
  $print["title"] = $node->title;
  $print["robots_meta"] = _print_robots_meta_generator();
  $print["base_href"] = "<base href=\"". url($path, array('absolute' => TRUE)) ."\" />\n";
  $print["head"] = drupal_get_html_head();
  $print["scripts"] = drupal_get_js();
  $print["favicon"] = theme_get_setting("toggle_favicon") ? "<link rel=\"shortcut icon\" href=\"". theme_get_setting("favicon") ."\" type=\"image/x-icon\"/>\n" : "";

  if (!empty($print_settings['css'])) {
    $css_file = $print_settings['css'];
  }
  else {
    $css_file = $base_url ."/". drupal_get_path('module', 'print') ."/print.css";
  }
  $print["css"] = "<style type=\"text/css\" media=\"all\">@import url(\"". $css_file ."\");</style>\n";

  $print["sendtoprinter"] = $print_html_settings['sendtoprinter'] ? " onload=\"window.print();\"" : "";
    
  $print["logo"] = !empty($print_settings['logo_url']) ? $print_settings['logo_url'] : theme_get_setting('logo');
  $print["logo"] = $print["logo"] ? "<img class=\"print-logo\" src=\"". $print["logo"] ."\" alt=\"\" />\n" : "";

  /* Grab and format the src URL */
  if (empty($print_sourceurl_settings['forcenode'])) {
    $print["url"] = url($path, array('absolute' => TRUE));
  }
  else {
    $print["url"] = $base_url .'/'. (((bool)variable_get('clean_url', '0')) ? '' : '?q=') . $path;
  }
  if ($cid) {
    $print["url"] .= "#comment-$cid";
  }
  if ($print_sourceurl_settings['enabled'] == 1) {
    $print["printdate"] = $print_sourceurl_settings['date'] ? (" (". $themed['retrieved'] ." ". format_date(time(), 'small') .")") : "";
    $print["source_url"] = "<strong>". $themed['sourceURL'] . $print["printdate"] .":</strong> <a href=\"". $print["url"] ."\">". $print["url"] ."</a>";
  }
  else {
    $print["source_url"] = "";
  }

  $nodetype = isset($node->type) ? $node->type : '';

  $print["site_name"] = variable_get('site_name', 0) ? ($themed['published'] ." ". variable_get('site_name', 0) ." (". l($base_url, $base_url) .")") : "";
  $print["submitted"] = theme_get_setting("toggle_node_info_$nodetype") ? $themed['by'] ." ". ($node->name ? $node->name : variable_get('anonymous', t('Anonymous'))) : "";
  $print["created"] = theme_get_setting("toggle_node_info_$nodetype") ? $themed['created'] ." ". format_date($node->created, 'small') : "";
  $print["footer_message"] = filter_xss_admin(variable_get('site_footer', FALSE)) ."\n". theme('blocks', 'footer') ;

  menu_set_active_item($path);
  $breadcrumb = drupal_get_breadcrumb();
  if (!empty($breadcrumb)) {
    $breadcrumb[] = menu_get_active_title();
    $print["breadcrumb"] = implode(" > ", $breadcrumb);
  }
  else {
    $print["breadcrumb"] = "";
  }

  // Display the collected links at the bottom of the page. Code once taken from Kjartan Mannes' project.module
  if (!empty($print_settings['urls'])) {
    $urls = _print_friendly_urls();
    $max = count($urls);
    $print["pfp_links"] = '';
    if ($max) {
      for ($i = 0; $i < $max; $i++) {
        $print["pfp_links"] .= '['. ($i + 1) .'] '. $urls[$i] ."<br />\n";
      }
      $print["pfp_links"] = "<p><strong>". $themed['links'] ."</strong><br />". $print["pfp_links"] ."</p>";
    }
  }


  if (module_exists('taxonomy')) {
    $terms = taxonomy_link('taxonomy terms', $node);
    $print["taxonomy"] = theme('links', $terms);
  }

  $print["type"] = $nodetype;
  $print["content"] = $node->body;

  return $print;
}

/**
 * We need to manipulate URLs so that they are recorded as absolute in the
 *  Printer-friendly URLs list, and to add a [n] footnote marker.
 *
 * @return string containing the changed <a> tag
 */
function _print_rewrite_urls($matches) {
  global $base_url, $base_root;

  // Get value of Printer-friendly URLs setting
  $print_settings = variable_get('print_settings', print_settings_default());
  $pfurls = (!empty($print_settings['urls']));

  //Temporarily convert spaces to %20 so that it isn't split below
  $in_string = false;
  for ($i=0; $i < strlen($matches[1]); $i++) {
    if ($matches[1][$i] == '"') {
      $in_string = !$in_string;
    }
    if (($matches[1][$i] == ' ') && ($in_string)) {
      $matches[1]=substr_replace($matches[1], "%20", $i, 1);
    }
  }
  // remove whitespace immediately before and after the '=' sign
  $matches[1]=preg_replace("/\s*=\s*/", "=", $matches[1]);

  // first, split the html into the different tag attributes
  $attribs = preg_split("/\s+/m", $matches[1]);

  for ($i=1; $i < count($attribs); $i++) {
    // If the attribute is href or src, we may need to rewrite the URL in the value
    if (preg_match("/^(href|src)\s*?=/i", $attribs[$i]) > 0) {
      // We may need to rewrite the URL, so let's isolate it
      preg_match("/.*?=(.*)/is", $attribs[$i], $urls);
      $url = trim($urls[1], " \t\n\r\0\x0B\"\'");

      if (strpos($url, '://') || preg_match("/^mailto:.*?@.*?\..*?$/iu", $url)) {
        // URL is absolute, do nothing
        $newurl = urldecode($url);
      }
      else {
        if (substr($url, 0, 1) == "#") {
          // URL is an anchor tag
          if ($pfurls) {
            $path = substr($_GET['q'], strlen(PRINT_PATH)+1);
            if (is_numeric($path)) {
              $path = "node/$path";
            }
            // Printer-friendly URLs is on, so we need to make it absolute
            $newurl = url($path, array('fragment' => substr(urldecode($url), 1), 'absolute' => TRUE));
          }
          // Because base href is the original page, change the link to
          // still be usable inside the print page
          $matches[1] = str_replace($url, $_GET['q'] . $url, $matches[1]);
        }
        else {
          // URL is relative, convert it into absolute URL
          $clean_url = (bool)variable_get('clean_url', '0');
          if (substr($url, 0, 1) == "/") {
            // If it starts with '/' just append it to the server name
            $newurl = $base_root .'/'. trim(urldecode($url), "/");
          }
          elseif ((!$clean_url) && (preg_match("/^[index.php]?\?q=.*/i", $url))) {
            // If Clean URLs is disabled, and it starts with q=?, just prepend with the base URL
            $newurl = $base_url .'/'. trim(urldecode($url), "/");
          }
          else {
            $newurl = url(trim(urldecode($url), "/"), array('absolute' => TRUE));
          }
          $matches[1] = str_replace($url, $newurl, $matches[1]);
        }
      }
    }
  }

  //Revert all %20 in strings back to spaces
  $matches[1] = str_replace("%20", " ", $matches[1]);

  $ret = '<'. $matches[1] .'>';
  if (count($matches) == 4) {
    $ret .= $matches[2] . $matches[3];
    if (($pfurls) && (isset($newurl))) {
      $ret .= ' <span class="print-footnote">['. _print_friendly_urls(trim(stripslashes($newurl))) .']</span>';
    }
  }

  return $ret;
}

/**
 * Auxiliary function to store the Printer-friendly URLs list as static.
 *
 * @return string containing the list of URLs previously stored if $url is 0,
 *         or the current count otherwise.
 */
function _print_friendly_urls($url = 0) {
  static $urls = array();
  if ($url) {
    $url_idx = array_search($url, $urls);
    if ($url_idx !== FALSE) {
      return ($url_idx + 1);
    }
    else {
      $urls[] = $url;
      return count($urls);
    }
  }
  return $urls;
}

/**
 * Auxiliary function to resolve the most appropriate template trying to find
 * a content specific template in the theme or module dir before falling back
 * on a generic template also in those dirs.
 *
 * @return string with the most suitable template filename
 */
function _print_get_template($type = NULL) {
  if ($type) {
    // If the node type is known, then try to find that type's template file
    // First in the theme directory
    $filename = drupal_get_path('theme', $GLOBALS['theme_key']) ."/print.node-$type.tpl.php";
    if (file_exists($filename)) {
      return $filename;
    }
    // Then in the module directory
    $filename = drupal_get_path('module', 'print') ."/print.node-$type.tpl.php";
    if (file_exists($filename)) {
      return $filename;
    }
  }
  // Search for a generic template file
  // First in the theme directory
  $filename = drupal_get_path('theme', $GLOBALS['theme_key']) ."/print.tpl.php";
  if (file_exists($filename)) {
    return $filename;
  }
  // Then in the module directory
  // This one must always exist (provided with the module!)
  return drupal_get_path('module', 'print') ."/print.tpl.php";
}

//*******************************************************************
// Module Functions : Content renderers
//*******************************************************************

/**
 * Outputs a printer-friendly page. Used for content types
 */
function _print_generate_node($nid, $cid = NULL) {
  global $base_url;

  // We can take a node id
  $node = node_load(array('nid' => $nid));
  if (!node_access('view', $node)) {
    // Access is denied
    return drupal_access_denied();
  }
  drupal_set_title($node->title);

  //alert other modules that we are generating a printer-friendly page, so they can choose to show/hide info
  $node->printing = true;
  // Turn off Pagination by the Paging module
  unset($node->pages);
  unset($node->page_count);

  unset($node->teaser);
  $node = (object)$node;
  if ($cid === NULL) {
    // Adapted (simplified) version of node_view for Drupal 5.x
    //Render the node content
    $node = node_build_content($node, false, true);
    // Disable fivestar widget output
    unset($node->content["fivestar_widget"]);
    // Disable service links module output
    unset($node->content["service_links"]);
  
    $node->body = drupal_render($node->content);
  }

  $print_settings = variable_get('print_settings', print_settings_default());

  if (function_exists('comment_render') && (($cid != NULL) || ($print_settings['comments']))) {
    //Print only the requested comment (or if $cid is NULL, all of them)
    $comments = comment_render($node, $cid);
    
    //Remove the comment forms
    $comments = preg_replace("/<form.*?id=\"comment-.*?\">.*?<\/form>/sim", "", $comments);
    //Remove the 'Post new comment' title
    $comments = preg_replace("/<h2.*?>Post new comment<\/h2>/", "", $comments);
    //Remove the comment title hyperlink
    $comments = preg_replace("/(<h3.*?>)(<a.*?>)(.*?)<\/a>(<\/h3>)/", "$1$3$4", $comments);
    //Remove the comment author link
    $comments = preg_replace("/(<span class=\"submitted\">)(.*?)<a.*?>(.*?)<\/a>(<\/span>)/sim", "$1$2$3$4", $comments);
    //Remove the comment links
    $comments = preg_replace("/\s*<ul class=\"links\">.*?<\/ul>/sim", "", $comments);
    if ($cid != NULL) {
      // Single comment requested, output only the comment
      unset($node->body);
    }
    $node->body .= $comments;
  }

  node_invoke_nodeapi($node, 'alter', false, true);

  // Convert the a href elements
  $pattern = "@<(a\s[^>]*?)>(.*?)(</a>)@is";
  $node->body = preg_replace_callback($pattern, "_print_rewrite_urls", $node->body);

  init_theme();

  $print = _print_var_generator($node, $cid);

  return $print;
}

/**
 * Outputs a printer-friendly page. Used for drupal core pages.
 */
function _print_generate_path($path) {
  global $base_url;

  $path = drupal_get_normal_path($path);

  menu_set_active_item($path);
  // Adapted from index.php.
  $node = new stdClass();
  $node->body = menu_execute_active_handler($path);
  $node->title = menu_get_active_title();
  $node->path = $path;

  // It may happen that a drupal_not_found is called in the above call
  if (preg_match('/404 Not Found/', drupal_get_headers()) == 1) {
    return;
  }

  switch ($node->body) {
  case MENU_NOT_FOUND:
    return drupal_not_found();
    break;
  case MENU_ACCESS_DENIED:
    return drupal_access_denied();
    break;
  }

  // Delete any links area
  $node->body = preg_replace("/\s*<div class=\"links\">.*?<\/div>/sim", "", $node->body);

  // Convert the a href elements
  $pattern = "@<(a\s[^>]*?)>(.*?)(</a>)@is";
  $node->body = preg_replace_callback($pattern, "_print_rewrite_urls", $node->body);

  init_theme();

  $print = _print_var_generator($node);

  return $print;
}


/**
 * Outputs a printer-friendly page. Used for book pages
 */
function _print_generate_book($nid) {
  global $base_url;

  $node = node_load(array('nid' => $nid));
  if (!node_access('view', $node) || (!user_access('access printer-friendly version'))) {
    // Access is denied
    return drupal_access_denied();
  }

  $tree = book_menu_subtree_data($node->book);
  $node->body = book_export_traverse($tree, 'book_node_export');

  // Convert the a href elements
  $pattern = "@<(a\s[^>]*?)>(.*?)(</a>)@is";
  $node->body = preg_replace_callback($pattern, "_print_rewrite_urls", $node->body);

  init_theme();

  $print = _print_var_generator($node);
  // The title is already displayed by the book_recurse, so avoid duplication
  $print["title"] = "";

  return $print;
}
