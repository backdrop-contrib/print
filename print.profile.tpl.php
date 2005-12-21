<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language; ?>" xml:lang="<?php print $language; ?>">

  <head>
    <title><?php print strip_tags($title); ?></title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <?php print $robots_meta; ?>
    <base href="<?php print $base_url ?>/" />
    <style type="text/css">
      @import url(<?php print $node->printcss; ?>);
    </style>
  </head>
  <body>

    <?php theme_get_setting('logo') ? print '<img src="'.theme_get_setting('logo').'" alt="logo" border="0" />' : '';?>

    <div class="source_url">
    <?php variable_get('site_name', 0) && print t('Published from').' '.variable_get('site_name', 0).' ('.l($base_url,'').')'; ?>
    </div>

    <h2 class="title"><?php print $title; ?></h2>

    <div class="content"><?php print $output; ?></div>

    <hr size="1" noshade="noshade" />

    <div class="source_url">
      <?php print '<strong>'.t('Source URL:').'</strong> <a href="'.$source_url.'>">'.$source_url.'</a>'?>
    </div>

    <div class="date-printed">
      <?php print '<strong>'.t('This page was printed on %date and is updated often.', array('%date' => format_date(time()))). '</strong>'; ?>
    </div>

    <div class="pfp-links">
      <!-- Output printer friendly links -->
      <?php $node->pfp_links ? print '<p class="links"><strong>'.t('Links:').'</strong><br />'.$node->pfp_links.'</p>' : ''; ?>
    </div>

    <div class="footer">
    </div>

  </body>
</html>