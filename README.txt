********************************************************************
                     D R U P A L    M O D U L E
********************************************************************
Name: Print module
Maintainer: Joao Ventura <joao at venturas dot org>
Author: Matt Westgate <drupal at asitis dot org>
Last update: (See CHANGELOG.txt for details)
Requires Drupal 5.x

********************************************************************
DESCRIPTION:

This module allows you to generate printer friendly versions of any node by navigating to www.example.com/print/nid, where nid is the node id of content to render.

A link is inserted in the each node (configurable in the content type settings), that opens a version of the page with no sidebars, search boxes, navigation pages, etc.

The following settings can be modified via the module settings page:

- Global enable/disable all links (overrides all content-type specific settings)
- Location of custom print-only logo and CSS file
- Explicit list of all URL references made on the page.
- Diplay of the node comments (if any)
- Ability to open page in a new window and/or print it automatically
- Optional display of original URL in aliased or fixed (node/nn) format, and optional page retrieval time.
- Ability to specify per-node robots settings: (no)index, (no)follow, noarchive, nocache

By editing the default print.css (or specifying you own CSS file) or the print.tpl.php files, it is possible to change the look of the output page to suit your taste. For a more fine-grained customization, it is possible to use a print.__node-type__.tpl.php file located in either the current theme or the module directory.

********************************************************************
INSTALLATION:

see the INSTALL.txt file in this directory.

********************************************************************
CONFIGURATION:

There are several settings that can be configured in the following places:

Administer › Site building › Modules (admin/build/modules)

  Enable or disable the module. (default: disabled)

Administer › User management › Access control (admin/user/access)

  Under print module:
  access print: Enable access to the PF page and display of the PF link in other pages. (default: disabled)
  administer print: Enable access to the module settings page. (default: disabled)

Administer › Content management › Content types (admin/content/types)

  For each content type it is possible to enable or disable the PF link via the "Show printer-friendly version link" checkbox. (default: enabled)

Administer › Content management › Comments › Settings (admin/content/comment/settings)

  It is possible to enabled or disable the PF link in individual comments via the "Show printer-friendly version link in individual comments" checkbox. (default: disabled)

Administer › Site configuration › Printer-friendly (admin/settings/print)

  This is where all the module-specific configuration options can be set:

  Printer-friendly page link (default: enabled)
  Enable or disable the printer-friendly page link for each node. Even if the link is disabled, you can still view the print version of a node by going to print/nid where nid is the numeric id of the node.

  Show link in system (non-content) pages (default: enabled)
  Setting this option will add a printer-friendly version page link on pages created by Drupal or the enabled modules.

  Logo URL (default: empty)
  An alternative logo to display on the printer-friendly version. If left empty, the current theme's logo is used.

  Stylesheet URL (default: empty)
  The URL to your custom print cascading stylesheet, if any. When none is specified, the default module CSS file is used.

  Printer-friendly URLs list (default: enabled)
  If set, a list of the destination URLs for the page links will be displayed at the bottom of the page.

  Include comments in print-friendly version (default: disabled)
  When this option is active, user comments are also included in the printer-friendly version. Requires the comment module.

  Open the printer-friendly version in a new window (default: disabled)
  Setting this option will make the printer-friendly version open in a new window/tab.

  Send to printer (default: disabled)
  Automatically calls the browser's print function when the printer-friendly version is displayed.

 Source URL:

  Display source URL (default: enabled)
  When this option is selected, the URL for the original page will be displayed at the bottom of the printer-friendly version.

  Add current time/date to the source URL (default: disabled)
  Display the current date and time in the Source URL line.

  Force use of node ID in source URL (default: disabled)
  Drupal will attempt to use the page's defined alias in case there is one. To force the use of the fixed URL, activate this option.

 Robots META tags:

  Add noindex (default: enabled)
  Instruct robots to not index printer-friendly pages. Recommended for good search engine karma.

  Add nofollow (default: enabled)
  Instruct robots to not follow outgoing links on printer-friendly pages.

  Add noarchive (default: disabled)
  Non-standard tag to instruct search engines to not show a "Cached" link for your printer-friendly pages. Recognized by Googlebot.

  Add nocache (default: disabled)
  Non-standard tag to instruct search engines to not show a "Cached" link for your printer-friendly pages

********************************************************************
Frequently Asked Questions:

1. I have enabled the module, but the PF page link is not showing up!

  There may be several reasons for this, but try the following in order (you must have appropriate permissions to perform some of these steps - login as user #1 if you don't):

    i. Make sure that the module is enabled (the checkbox next to 'Printer-friendly pages' in admin/build/modules must be ticked).

   ii. In admin/user/access under the print module heading make sure that you have enabled access to the desired groups. In most cases, you will want to tick the 'access print' boxes for all groups, and clear the 'admin print' boxes for all groups except the administrator group (if defined).

  iii. In admin/settings/print, make sure that you have enabled the PF link.

   iv. For each content type x, check in admin/content/types/x that the 'Show printer-friendly version link' is enabled.

2. The Printer-friendly version in book nodes is not working correctly..

  That PF link is managed by the book module and this module has nothing to do with it.

3. Can I replace the 'Printer-friendly version' link with an image?

  Sure you can.. But you will have to change your theme's CSS pages for that. Define the following:

  .print-page {
    background:url(__icon_url__) no-repeat;
    width:__icon_width__px;
    height:__icon_height__px;
    font-size: 0px;
  }

4. Is it possible to change the styles used in the PF page?

  You can always change them easily. You can either:

  a) Edit the print.css in the modules directory, or
  b) Define your own CSS with the appropriate styles and specify it in the module settings.

5. Is it possible to define a template for a specific content type?

  Sure you can, the module will try the following locations for the PF page template:

    i. print.__node-type__.tpl.php in the theme directory
   ii. print.__node-type__.tpl.php in the module directory
  iii. print.tpl.php in the theme directory
   iv. print.tpl.php in the module directory (supplied by the module)

  I recommend that you edit the module's print.tpl.php to suit your tastes instead of creating something from scratch, as it is easier to remember the name of the members of the $print array and the names of the used styles.

  Note that since you are creating your own template, if you want to use per-content-type styles, you can simply ignore the $print['css'] setting and hard-code your own per-content-type CSS file in your new template. Or you can define your own per-content-type styles to store in the configurable CSS file. Your choice!

6. I don't like the template used. Can I use another?

  Of course, see the above question on how to do it (or you can always simply edit the module's print.tpl.php).

7. I am a module author/maintainer. Is there a way to know when a node is being built by the print module so that I can disable my output?

  Yes, if $node->printing evaluates to true, disable any output that is not suitable for a static page.

8. Is it possible to place the PF link somewhere else?

  Yes, but you have to take care of it. Disable the PF link in admin/settings/print and you're then free to place a link to print/nid or to print/path_alias wherever you want (such as a block or the node content). I recommend the following code to place the link, as it maintains the configurable attributes:

  <?php print print_insert_link(); ?>

9. I have a problem/question not answered here. Where can I go for help?

  Simply post a new issue in Drupal's issue system: http://drupal.org/project/issues/print
  If possible, send me an e-mail with the link to your Drupal system where I can see the problem you are reporting.

