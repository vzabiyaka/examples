<?php

/**
 * @file
 * MF Homepage Lear it, Earn it, Bearn it block template.
 */
?>

<?php if (!empty($items)): ?>
  <?php if (!empty($sponsored_logo)) : ?>
    <?php print render($sponsored_logo); ?>
  <?php endif; ?>
  <section class="leb-block cf">
    <div class="page-wrapper">
      <div class="leb-block__inner">
        <?php foreach($items as $item): ?>
          <div class="leb-block__item">
            <div class="media-block media-block media-block_tile">
              <div class="media-block__headline">
                <?php if (!empty($item['flag'])): ?>
                  <strong class="media-block__headline-promo"><?php print $item['flag']; ?></strong>
                <?php endif; ?>
                <?php if (!empty($item['title'])): ?>
                  <span class="media-block__headline-text"><?php print $item['title']; ?></span>
                <?php endif; ?>
              </div>

              <div class="media-block__meta">
                <span class="media-block__byline">
                  <?php print $item['byline']; ?>
                </span>
              </div>

              <?php if (!empty($item['path']) && !empty($item['image'])): ?>
                <a href="/<?php print $item['path']; ?>">
                  <div class="media-block__media">
                    <?php print $item['image']; ?>
                  </div>
                </a>
              <?php endif; ?>

              <?php if (!empty($item['description'])): ?>
                <div class="media-block__body">
                  <p class="media-block__expert">
                    <?php print $item['description']; ?>
                  </p>
                </div>
              <?php endif; ?>
            </div>
          </div>
        <?php endforeach; ?>
      </div>
    </div>
  </section>
<?php endif; ?>
