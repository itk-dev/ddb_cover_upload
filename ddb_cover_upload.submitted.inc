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
  $image = _ddb_cover_upload_fetch_submitted_image($data);
  $file = file_load($data['fid']);
  return array(
    '#attached' => array (
      'css' => array(
        drupal_get_path('module', 'ddb_cover_upload') . '/css/ddb_cover_upload.css',
      ),
      'js' => array(
        drupal_get_path('module', 'ddb_cover_upload') . '/js/ddb_cover_upload_check_submission.js',
      ),
    ),
    '#markup' => theme('ddb_cover_upload_submitted', array(
      'data' => $data,
      'image' => $file ? $image : NULL,
    ))
  );
}

/**
 * Check cover service to determine if file has been uploaded.
 * This function is called continuously through ajax.
 * @todo Proper check against cover service. Currently just reports success after a while.
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
 * Fetch the image that was submitted. Either an altered version or original.
 *
 * @param array $data
 *   The ddb_cover_upload data from session.
 *
 * @return bool|string
 *   A URL to the image the user decided to upload.
 */
function _ddb_cover_upload_fetch_submitted_image(array $data){
  if ($data['image_altered']) {
    return(file_create_url(file_build_uri('/upload_cover_service/' . $data['fid'] . '/' . $data['fid'] . '.jpg')));
  }
  else {
    $uploaded = file_load($data['fid']);
    return(file_create_url($uploaded->uri));
  }
}

/**
 * Remove db entry and files locally.
 *
 * @param array $data
 *   The ddb_cover_upload data from session.
 */
function _ddb_cover_upload_cleanup(array $data) {
  $file = file_load($data['fid']);
  if ($file) {
    $public_uri = variable_get('file_public_path','sites/default/files');
    if ($data['image_altered']) {
      $alteredImageDir = $public_uri . '/upload_cover_service/' . $data['fid'];
      file_unmanaged_delete($alteredImageDir . '/' . $data['fid'] . '.jpg');
      drupal_rmdir($alteredImageDir);
    }

    file_delete(file_load($data['fid']));
  }
}
