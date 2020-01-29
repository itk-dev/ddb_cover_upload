<?php
/**
 * @file
 * Autocomplete functionality for ting reference related to cover upload.
 */

/**
 * Create an alternate image of original upload and save it under fid folder as
 * [fid].jpg.
 * This function is called when an image cropping is completed.
 * @see ddb_cover_upload_crop.js
 */
function _cover_upload_create_alt_image() {
  // Extract fid from filename.
  $file_name = $_FILES['croppedImage']['name'];
  $file_name_exploded = explode('.', $file_name);
  $files_path = 'sites/default/files/upload_cover_service/';
  // @todo Maybe change the folder permissions.
  mkdir($files_path . $file_name_exploded[0], 0700);
  $file_to_upload = $_FILES['croppedImage']['tmp_name'];
  $file_destination = $files_path . $file_name_exploded[0] . '/' . $file_name;
  move_uploaded_file($file_to_upload, $file_destination);
}
