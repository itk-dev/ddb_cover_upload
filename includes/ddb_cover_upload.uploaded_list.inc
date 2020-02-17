<?php
/**
 * @file
 * List display of uploaded covers from this library.
 */

/**
 * Display list of covers uploaded by the library.
 *
 * @return array
 *   A themed list.
 * @throws \Exception
 *   Theme exception error.
 */
function ddb_cover_upload_list() {
  // @todo get list of uploaded covers.
  drupal_set_title(t('List uploaded covers'));
  $header = array(
    t('Image'),
    t('Material'),
    t('Actions')
  );
  $rows = array();
  return array(
    '#markup' => theme('table', array(
      'header' => $header,
      'rows' => $rows,
    ))
  );
}
