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

  // @TODO pager on tables?

  $rows = array();

  $materials = _ddb_cover_upload_get_materials();

  foreach ($materials as $material) {
    $cover_ref = $material->getCover();
    $id = _ddb_cover_upload_parse_cover_id($cover_ref);
    $cover = _ddb_cover_upload_get_cover($id);
    $image = array(
      '#theme' => 'image',
      '#path' => $cover->getImageUrl(),
      '#width' => '120px',
    );

    /** @var \Ting\TingObject $object */
    $object = ding_entity_load($material->getIsIdentifier(), 'ting_object');
    $uri = ding_entity_uri('ding_entity', $object);

    $rows[] = [
      $material->getId(),
      render($image),
      $object->getTitle() . ' (' . $object->getType() .')',
      l($material->getIsIdentifier(), $uri['path'], array('html' => TRUE)),
      l(t('Delete'), 'admin/config/cover_upload/upload/list/delete/' . $material->getId(), array('html' => TRUE)),
    ];
  }

  // @todo get list of uploaded covers.
  drupal_set_title(t('List uploaded covers'));
  $header = array(
    t('Id'),
    t('Image'),
    t('Title'),
    t('Material'),
    t('Actions')
  );

  return array(
    '#markup' => theme('table', array(
      'header' => $header,
      'rows' => $rows,
    ))
  );
}

function _ddb_cover_upload_parse_cover_id($ref) {
  $parts = explode('/', $ref);
  return array_pop($parts);
}

function _ddb_cover_upload_get_materials($page = 1) {
  // @TODO: cache
  $config = ddb_cover_upload_client_config();
  $apiInstance = new CoverServiceUpload\Api\MaterialApi(
    new GuzzleHttp\Client(),
    $config
  );

  return $apiInstance->getMaterialCollection($page);
}

function _ddb_cover_upload_get_cover($id) {
  // @TODO: cache
  $config = ddb_cover_upload_client_config();
  $apiInstance = new CoverServiceUpload\Api\CoverApi(
    new GuzzleHttp\Client(),
    $config
  );

  return $apiInstance->getCoverItem($id);
}
