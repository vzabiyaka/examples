<?php

/**
 * @file
 * MFConfigBlocks class file.
 */

/**
 * Class to process and display nodequeue based blocks.
 *
 * Class MFConfigBlocks.
 */
class MFConfigBlocks extends MFBlocks {

  const MF_BLOCKS_NODEQUEUE_TYPE = 1;

  const MF_BLOCKS_CONTENT_TYPE = 2;

  const MF_BLOCKS_TERM_TYPE = 3;

  /**
   * Common configuration form builder for homepage nodequeue based blocks.
   *
   * @param string $delta
   *   Block delta.
   *
   * @return array
   *   Form settings array.
   */
  public static function getBlockConfForm($delta) {
    $form = array();
    $variable = variable_get($delta);
    // Set default block types select.
    $form['block_type'] = array(
      '#type' => 'select',
      '#title' => t('Block Type'),
      '#description' => t('There are two main types: Nodequeue based block and Latest Content types based'),
      '#default_value' => !empty($variable['block_type']) ? $variable['block_type'] : '1',
      '#options' => array(
        self::MF_BLOCKS_CONTENT_TYPE => 'Content type based (latest)',
        self::MF_BLOCKS_TERM_TYPE => 'Taxonomy term based (latest)',
      ),
      '#required' => TRUE,
    );

    $nodequeues = nodequeue_get_qid_map();
    // Set nodequeue based block type only if nodequeues present.
    if (!empty($nodequeues)) {
      $nodequeue_type = array(self::MF_BLOCKS_NODEQUEUE_TYPE => 'Nodeque based');
      $form['block_type']['#options'] = $nodequeue_type + $form['block_type']['#options'];

      $nodequeues = array_keys($nodequeues);
      $form[$delta . '_nodequeue'] = array(
        '#type' => 'select',
        '#title' => t('Nodequeue to use'),
        '#default_value' => !empty($variable['nodequeue']) ? $variable['nodequeue'] : '-select-',
        '#options' => drupal_map_assoc($nodequeues),
        '#required' => TRUE,
        '#states' => array(
          'visible' => array(
            'select[name="block_type"]' => array('value' => self::MF_BLOCKS_NODEQUEUE_TYPE),
          ),
        ),
      );
    }
    // Add supported node types.
    $checkboxes = self::getNodeTypes();
    $form[$delta . '_content_types'] = array(
      '#type' => 'checkboxes',
      '#options' => $checkboxes,
      '#title' => t('Select CT to support'),
      '#description' => t('Only CTs above have corresponding view modes configured to be used in smart blocks.'),
      '#default_value' => !empty($variable['content_types']) ? array_values($variable['content_types']) : array(),
      '#states' => array(
        'visible' => array(
          'select[name="block_type"]' => array('value' => self::MF_BLOCKS_CONTENT_TYPE),
        ),
      ),
    );

    $terms = self::getTaxonomyTerms();
    $form[$delta . '_taxonomy_term'] = array(
      '#type' => 'checkboxes',
      '#options' => $terms,
      '#title' => t('Select taxonomy term support'),
      '#description' => t('Only nodes of supported CT with taxonomy reference will be added to this block.'),
      '#default_value' => !empty($variable['taxonomy_term']) ? $variable['taxonomy_term'] : '',
      '#states' => array(
        'visible' => array(
          'select[name="block_type"]' => array('value' => self::MF_BLOCKS_TERM_TYPE),
        ),
      ),
    );

    $form[$delta . '_quantity'] = array(
      '#type' => 'textfield',
      '#title' => t('Number of recent content items to display'),
      '#default_value' => !empty($variable['quantity']) ? $variable['quantity'] : '5',
      '#required' => TRUE,
    );

    if (in_array($delta, array('mf_homepage_leb', 'mf_homepage_body_book_block'))) {
      $options = array('' => '-NONE-');
      $terms = entity_load('taxonomy_term', FALSE, array('vid' => 16));
      foreach ($terms as $tid => $term) {
        $options[$tid] = $term->name;
      }
      $form['sponsored_logo'] = array(
        '#type' => 'select',
        '#title' => t('Sponsored by logo'),
        '#description' => t('Select sponsor'),
        '#default_value' => !empty($variable['sponsored_logo']) ? $variable['sponsored_logo'] : '',
        '#options' => $options,
      );
    }
    return $form;
  }

  /**
   * Common configuration form builder for homepage nodequeue based blocks.
   *
   * @param string $delta
   *   Block delta.
   *
   * @return array
   *   Form settings array.
   */
  public static function getPromoBannersBlockConfForm($delta) {
    $form = array();
    $variable = variable_get('mf_homepage_promo_banners', array(
      'first_block' => array(
        'desktop' => '',
        'mobile' => '',
      ),
      'second_block' => array(
        'desktop' => '',
        'mobile' => '',
      ),
    ));
    // Set default block types select.
    $form['main_settings'] = array(
      '#type' => 'fieldset',
      '#title' => 'Promo Banners',
    );
    $form['main_settings']['desktop'] = array(
      '#type' => 'fieldset',
      '#title' => 'Desktop Promo Banners',
    );
    $form['main_settings']['desktop']['desktop_textearea_1'] = array(
      '#type' => 'textarea',
      '#title' => 'First Desktop Banner',
      '#rows' => 5,
      '#default_value' => $variable['first_block']['desktop'],
    );
    $form['main_settings']['desktop']['desktop_textearea_2'] = array(
      '#type' => 'textarea',
      '#title' => 'Second Desktop Banner',
      '#rows' => 5,
      '#default_value' => $variable['second_block']['desktop'],
    );
    $form['main_settings']['mobile'] = array(
      '#type' => 'fieldset',
      '#title' => 'Mobile Promo Banners',
    );
    $form['main_settings']['mobile']['mobile_textearea_1'] = array(
      '#type' => 'textarea',
      '#title' => 'First Mobile Banner',
      '#rows' => 5,
      '#default_value' => $variable['first_block']['mobile'],
    );
    $form['main_settings']['mobile']['mobile_textearea_2'] = array(
      '#type' => 'textarea',
      '#title' => 'Second Mobile Banner',
      '#rows' => 5,
      '#default_value' => $variable['second_block']['mobile'],
    );
    
    return $form;
  }

  /**
   * Helper to save nodequeue based blocks settings.
   *
   * @param string $delta
   *   Block delta.
   * @param array $edit
   *   Block settings form submit array.
   */
  public static function setBlockSettings($delta, $edit) {
    $variable = array();
    // Set keys according to block types.
    if (!empty($edit['block_type'])) {
      $variable['block_type'] = $edit['block_type'];
      if ($edit['block_type'] == self::MF_BLOCKS_NODEQUEUE_TYPE) {
        if (!empty($edit[$delta . '_nodequeue'])) {
          $variable['nodequeue'] = $edit[$delta . '_nodequeue'];
        }
      }
      elseif ($edit['block_type'] == self::MF_BLOCKS_CONTENT_TYPE) {
        if (!empty($edit[$delta . '_content_types'])) {
          $variable['content_types'] = array_filter($edit[$delta . '_content_types']);
        }
      }
      elseif ($edit['block_type'] == self::MF_BLOCKS_TERM_TYPE) {
        if (!empty($edit[$delta . '_taxonomy_term'])) {
          $variable['taxonomy_term'] = $edit[$delta . '_taxonomy_term'];
        }
      }
    }

    if (in_array($delta, array('mf_homepage_leb', 'mf_homepage_body_book_block')) && isset($edit['sponsored_logo'])) {
      $variable['sponsored_logo'] = $edit['sponsored_logo'];
    }
    if (!empty($edit[$delta . '_quantity'])) {
      $variable['quantity'] = $edit[$delta . '_quantity'];
    }

    variable_set($delta, $variable);
  }
  
  /**
   * Helper to save promo banners block settings.
   *
   * @param string $delta
   *   Block delta.
   * @param array $edit
   *   Block settings form submit array.
   */
  public static function setPromoBannersBlockSettings($delta, $edit) {
    $variable = array();
    // Set keys according to block types.
    $variable = array(
      'first_block' => array(
        'desktop' => $edit['desktop_textearea_1'],
        'mobile' => $edit['mobile_textearea_1'],
      ),
      'second_block' => array(
        'desktop' => $edit['desktop_textearea_2'],
        'mobile' => $edit['mobile_textearea_2'],
      ),
    );

    variable_set('mf_homepage_promo_banners', $variable);
  }

  /**
   * Helper to get node types options.
   *
   * @return array
   *   Options node types array.
   */
  private static function getNodeTypes() {
    // We process only CT which are covered in MFLoadEntities.
    // All of them has created display modes to be displayed properly.
    $node_types = node_type_get_names();
    $covered_node_types = MFLoadEntities::$defaultNodeTypes;
    $checkboxes = array();
    foreach ($node_types as $key => $node_type) {
      if (!in_array($key, $covered_node_types)) {
        unset($node_types[$key]);
      }
      else {
        $checkboxes[$key] = $node_type;
      }
    }

    return $checkboxes;
  }

  /**
   * Helper to get taxonomy terms options.
   *
   * @return array
   *   Options primary_categories taxonomy terms array.
   */
  private static function getTaxonomyTerms() {
    $checkboxes = array();
    $vocabulary = taxonomy_vocabulary_machine_name_load('primary_categories');
    $terms = taxonomy_get_tree($vocabulary->vid);
    foreach ($terms as $key => $term) {
      // Get the parent TID(s) of the terms.
      $parents = current($term->parents);
      // If there is no parent, remove the term from our list.
      if ($parents == '0') {
        $checkboxes[$term->tid] = $term->name;
      }
    }

    return $checkboxes;
  }

  /**
   * Helper to get block nodes.
   *
   * @param string $delta
   *   Block delta.
   *
   * @return array
   *   Nodes array.
   */
  protected static function getBlockNodes($delta) {
    // Get block settings.
    $settings = variable_get($delta);
    $nodes = array();
    // Check if nodequeue settings selected.
    if (!empty($settings['nodequeue'])) {
      $nodes = MFLoadEntities::getNodequeueNodes($settings['nodequeue']);
    }
    elseif (!empty($settings['content_types'])) {
      $content_types = array_values($settings['content_types']);
      $nodes = MFLoadEntities::preloadNids($settings['quantity'], $content_types);
    }
    elseif (!empty($settings['taxonomy_term'])) {
      $tids = array_keys($settings['taxonomy_term']);
      $tids = self::getTermChildrenTids($tids);
      // Handle issue with homepage blocks order.
      $offset = NULL;
      $homepage_offsets = array(
        'mf_homepage_training_left',
        'mf_homepage_women_left',
        'mf_homepage_recipes_left',
      );
      if (in_array($delta, $homepage_offsets)) {
        $offset = 3;
      }
      elseif ($delta == 'mf_homepage_lifestyle_left') {
        $offset = 1;
      }

      $nodes = MFLoadEntities::preloadNids($settings['quantity'], NULL, $tids, $offset);
    }

    if (!empty($settings['quantity'])) {
      $quantity = $settings['quantity'];
      $nodes_quantity = count($nodes);
      // Preload all other nodes without previous 5 videos.
      if ($nodes_quantity > $quantity) {
        // Get exact quantity of nodes when needed.
        // And preserve keys.
        $nodes = array_slice($nodes, 0, $quantity, TRUE);
      }
    }

    return $nodes;
  }

}
