<?php
// $Id$

// Extend the TCPDF class to create custom Header and Footer
class PrintTCPDF extends TCPDF {
  protected $footer;

  //Page footer
  public function SetFooterData($arg = '') {
    $this->footer = $arg;
  }

  //Page footer
  public function Footer() {
    //Position at 1.5 cm from bottom
    $this->writeHTMLCell(0, 15, 0, -15, $this->footer, 0, 0, 0, TRUE, 'C');

    $ormargins = $this->getOriginalMargins();
    $pagenumtxt = t('Page') .' '. $this->PageNo() .' / {nb}';
    //Print page number
    if ($this->getRTL()) {
      $this->SetX($ormargins['right']);
      $this->Cell(0, 0, $pagenumtxt, 'T', 0, 'L');
    } else {
      $this->SetX($ormargins['left']);
      $this->Cell(0, 10, $pagenumtxt, 'T', 0, 'R');
    }
  }
}
