<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $node->language; ?>" xml:lang="<?php print $node->language; ?>">

  <head>
    <title><?php print $node->title; ?></title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <base href="<?php print $base_url ?>/" />
    <style type="text/css">
      @import url "<?php print $node->printcss; ?>";
    </style>
  </head>
  <body>

    <h1 class="title">
      <?php print $node->title; ?>
    </h1>

    <div class="submitted">
      By <?php print $node->name; ?>
    </div>

    <div class="created">
      Created <?php print format_date($node->created, 'small'); ?>
    </div>

    <div class="content">
      <?php print $node->body; ?>
    </div>

    <div class="pfp-links">
      <!-- Output printer friendly links -->
      <h2 class="links">Links</h2>
      <?php print $node->pfp_links; ?>
    </div>

    <div class="source_url">
      <strong>Source URL:</strong> <a href="<?php print $node->source_url; ?>"><?php print $node->source_url; ?></a>
    </div>

    <div class="footer">
      <!-- Add your custom footer here. -->
    </div>

  </body>
</html>
