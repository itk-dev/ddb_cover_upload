<?php
/**
 * @file
 * Cover upload page when form is submitted.
 */

/**
 * Display cover submitted page.
 *
 * @return array
 *   A themed page.
 * @throws \Exception
 *   Theme exception error.
 */
function ddb_cover_upload_submitted() {
  $data = _ddb_cover_upload_session('ddb_cover_upload_submitted');
  $image_url = file_create_url($data['image_uri']);

  return array(
    '#attached' => array (
      'css' => array(
        drupal_get_path('module', 'ddb_cover_upload') . '/css/ddb_cover_upload.admin.css',
      ),
      'js' => array(
        drupal_get_path('module', 'ddb_cover_upload') . '/js/ddb_cover_upload_check_submission.admin.js',
      ),
    ),
    '#markup' => theme('ddb_cover_upload_submitted', array(
      'data' => $data,
      'image' => $image_url,
    ))
  );
}

/**
 * Check cover service to determine if file has been uploaded.
 *
 * This function is called continuously through ajax.
 * @todo Proper check against cover service. Currently just reports success after a while.
 *
 * @see ddb_cover_upload_check_submission.js
 */
function _cover_upload_check_submission() {
  $data = _ddb_cover_upload_session('ddb_cover_upload_submitted');
  $image = _ddb_cover_upload_fetch_submitted_image($data);
  $state = array(
    'status' => 'waiting',
  );

  // @todo remove this if/else condition when proper check is implemented.
  if(empty(_ddb_cover_upload_session('ddb_cover_upload_time'))) {
    // @todo remove this line when proper check is implemented.
    _ddb_cover_upload_session('ddb_cover_upload_time', time());
  }
  else {
    // @todo remove this line when proper check is implemented.
    $timepassed = time() - _ddb_cover_upload_session('ddb_cover_upload_time');
    // If image is uploaded.
    // @todo Change this line and implement a proper check.
    if ($timepassed > 3) {
      // Notify frontend
      $state['status'] = 'success';
      drupal_json_output($state);
      // @todo remove this line when proper check is implemented.
      unset($_SESSION['ddb_cover_upload_time']);
      _ddb_cover_upload_cleanup($data);
      drupal_exit();
    }
  }

  // Notify frontend "Nothing to see yet".
  drupal_json_output($state);
  drupal_exit();
}

/**
 * Remove db entry and files locally.
 *
 * @param array $data
 *   The ddb_cover_upload data from session.
 */
function _ddb_cover_upload_cleanup(array $data) {

}
