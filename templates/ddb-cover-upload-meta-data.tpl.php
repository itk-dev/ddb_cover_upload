<div>
  <h3>Image metadata</h3>
  <div class="content">
    <div class="field field-label-inline">
      <div class="field-label"><?php print t('Image name') . ':';?>&nbsp;</div>
      <div><?php print $file_data->filename;?></div>
    </div>
    <div class="field field-label-inline">
      <div class="field-label"><?php print t('Image dimensions') . ':';?>&nbsp;</div>
      <div><?php print $meta_data['file_size']['x'] . 'X' . $meta_data['file_size']['y'];?></div>
    </div>
    <div class="field field-label-inline">
      <div class="field-label"><?php print t('Image file type') . ':';?>&nbsp;</div>
      <div><?php print $file_data->filemime;?></div>
    </div>
    <div class="field field-label-inline">
      <div class="field-label"><?php print t('Image file size') . ':';?>&nbsp;</div>
      <div><?php print $file_data->filesize / 1000;?>KB</div>
    </div>
    <div class="field field-label-inline">
      <div class="field-label"><?php print t('Author') . ':';?>&nbsp;</div>
      <div><?php print $meta_data['author']?></div>
    </div>
  </div>
</div>
