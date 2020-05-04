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
 */
function _cover_upload_check_submission() {
  $data = _ddb_cover_upload_session('ddb_cover_upload_submitted');

  // Check if the minimum required data exists.
  if (!isset($data['image_uri']) || !isset($data['pid'])) {
    drupal_json_output(array(
      'message' => t('Either the image have been uploaded ot something is wrong with the session'),
      'status' => 'error',
    ));
    drupal_exit();
  }

  try {
    _ddb_cover_upload_client_upload($data['image_uri'], $data['pid'], 'pid');
  }
  catch (\CoverServiceUpload\ApiException $e) {
    drupal_json_output(array(
      'message' => $e->getMessage(),
      'status' => 'error',
    ));
    drupal_exit();
  }

  // If we get here the image should have been upload and we clean up the local
  // installation.
  _ddb_cover_upload_cleanup();

  // Notify frontend "Nothing to see yet".
  drupal_json_output($state = array(
    'status' => 'success',
  ));
  drupal_exit();
}

/**
 * Remove db entry and files locally.
 */
function _ddb_cover_upload_cleanup() {
  $data = _ddb_cover_upload_session('ddb_cover_upload_submitted');

  // Mark image as upload to prevent more uploads of the same data.
  _ddb_cover_upload_session('ddb_cover_upload_submitted', '');

  // Remove local copy of the image.
  file_unmanaged_delete($data['image_uri']);
}

/**
 * Wrapper to upload a cover to the service.
 *
 * @param string $image_uri
 *   The local URI for the cover image.
 * @param string $identifier
 *   The identifier for the material for the cover.
 * @param string $identifier_type
 *   The type of identifier (faust, pid, isbn).
 *
 * @throws \CoverServiceUpload\ApiException
 */
function _ddb_cover_upload_client_upload(string $image_uri, string $identifier, string $identifier_type) {
  $image = drupal_realpath($image_uri);
  $config = _ddb_cover_upload_client_config();
  $apiInstance = new CoverServiceUpload\Api\CoverApi(
    new GuzzleHttp\Client(),
    $config
  );

  /** @var \CoverServiceUpload\Model\CoverCoverRead $cover */
  $cover = $apiInstance->postCoverCollection($image);

  $material = new \CoverServiceUpload\Model\MaterialWrite();
  $material->setCover('/api/covers/'.$cover->getId());
  $material->setIsType($identifier_type);
  $material->setIsIdentifier($identifier);

  $apiInstance = new CoverServiceUpload\Api\MaterialApi(
    new GuzzleHttp\Client(),
    $config
  );
  $apiInstance->postMaterialCollection($material);
}

/**
 * Helper to get configuration for the upload API.
 *
 * @return \CoverServiceUpload\Configuration
 *   API configuration.
 */
function _ddb_cover_upload_client_config() {
  $config = CoverServiceUpload\Configuration::getDefaultConfiguration();

  // Get access token for the library.
  $token = _ddb_cover_upload_get_library_token();
  $config->setAccessToken($token);

  $config->setHost(variable_get('ddb_cover_upload_host', 'https://upload.cover.dandigbib.org'));

  return $config;
}
