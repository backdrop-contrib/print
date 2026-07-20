/**
 * @file
 * Sets the summary for Print PDF on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.printPdfSettingsSummary = {
  attach: function(context) {
    var $context = $(context);
    $context.find('fieldset#edit-print-pdf').backdropSetSummary(function () {
      var vals = [];

      if ($('select[name="print_pdf_size"]', context).val()) {
        vals.push('Size ' + Backdrop.checkPlain($('select[name="print_pdf_size"] option:selected').text()));
      }
      else {
        vals.push(Backdrop.t('Size unchanged'));
      }

      if ($('select[name="print_pdf_orientation"]', context).val()) {
        vals.push(Backdrop.checkPlain($('select[name="print_pdf_orientation"] option:selected').text()));
      }

      return vals.join(', ');
    });
  }
};

})(jQuery);
