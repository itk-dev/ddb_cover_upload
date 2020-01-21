<div class="overflow-hidden">
  <p>
    <a href="#" class="btn btn-primary opener-modal" data-dialog="dialog-image"><?php print t('Adapt image'); ?></a>
  </p>
  <!-- @TODO: Placehold image only for class demo purpose. -->
  <?php print $variables['image']; ?>
</div>
<div tabindex="-1" role="dialog-image" style="display: none;">
  <div id="dialog-image" class="ui-dialog-content ui-widget-content" data-title="<?php print t('Adapt image'); ?>">
    --- Do image magic here ---
  </div>
</div>
