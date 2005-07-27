<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $node->language; ?>" xml:lang="<?php print $node->language; ?>">

  <head>
    <title><?php print $node->title; ?></title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <base href="<?php print $base_url ?>/" />
    <style type="text/css">
      @import url(<?php print $node->printcss; ?>);
    </style>
  </head>
  <body>

    <?php theme_get_setting('logo') ? print '<img src="'.theme_get_setting('logo').'" alt="logo" border="0" />' : '';?>

    <div class="source_url">
    <?php variable_get('site_name', 0) && print t('Published on').' '.variable_get('site_name', 0).' ('.l($base_url,'').')'; ?>
    </div>

    <h2 class="title">
      <?php print $node->title; ?>
    </h2>

    <div class="submitted">
      <?php print theme_get_setting("toggle_node_info_$node->type") ? t('By').' '.$node->name : ''; ?>
    </div>

    <div class="created">
      <?php print theme_get_setting("toggle_node_info_$node->type") ? t('Created').' '.format_date($node->created, 'small') : '' ?>
    </div>

    <div class="content">
      <?php print $node->body; ?>
    </div>

    <hr size="1" noshade>

    <div class="source_url">
      <?php print '<strong>'.t('Source URL:').'</strong><br><a href="'.$node->source_url.'>">'.$node->source_url.'</a>'?>
    </div>

    <div class="pfp-links">
      <!-- Output printer friendly links -->
      <?php $node->pfp_links ? print '<p class="links"><strong>'.t('Links:').'</strong><br>'.$node->pfp_links.'</p>' : ''; ?>
    </div>

    <div class="footer">
      <!-- Add your custom footer here. -->
    </div>

  </body>
</html>
