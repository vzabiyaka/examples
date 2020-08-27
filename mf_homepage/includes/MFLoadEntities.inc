<?php
/**
 * @file
 * MFLoadEntities class file.
 */

/**
 * Helper class to handle loaded nodes mutual exclusivity.
 *
 * Also handles nodequeues node load functionality of custom blocks.
 * This class is widely used inside methods of MFBlocks class and sub classes.
 *
 * Class MFLoadEntities
 */
class MFLoadEntities {

  const QUANTITY_OF_PRELOADED_NIDS = 10;

  public static $defaultNodeTypes = array(
    'article',
    'product',
    'photo_gallery',
    'recipe',
    'sponsored_page',
    'sponsored_page_two',
    'video',
    'webform',
    'workout',
    'workout_routine',
  );

  /**
   * Preload node ids to use as placeholder when nodequeues size is not enough.
   *
   * It is better to execute one query then create multiple with condition
   * NOT IN.
   *
   * @param int|string $quantity
   *   Quantity of items.
   * @param array $types
   *   Node types array.
   * @param int|string $tids
   *   Term ids.
   * @param int|string $offset
   *   Off ids.
   * @param array $not_in
   *   Loaded node ids array.
   * @param bool $not_sponsored
   *   Flag to load or not sponsored nodes.
   *
   * @return array|mixed
   *   Node ids array keyed by loaded types.
   */
  public static function preloadNids($quantity = NULL, $types = NULL, $tids = NULL, $offset = NULL, $not_in = array(), $not_sponsored = NULL) {
    // Set static cache for loaded nids.
    $loaded = &drupal_static(__FUNCTION__);
    if (!isset($loaded) || !is_array($loaded)) {
      // Use mutual exclusivity to nodequeue nodes only for frontpage.
      if (drupal_is_front_page()) {
        $loaded = self::getHomepageNodequeueNodes();
      }
      else {
        $loaded = $not_in;
      }
    }

    // Set node types.
    if (empty($types)) {
      $types = self::$defaultNodeTypes;
    }
    // Set quantity of nodes.
    if (empty($quantity)) {
      $quantity = self::QUANTITY_OF_PRELOADED_NIDS;
    }

    $nodes = array();

    // Build query.
    $query = db_select('node', 'n')
      ->fields('n', array('nid'))
      ->condition('type', $types, 'IN')
      ->condition('n.status', NODE_PUBLISHED);

    // Add not in condition if some nodes already loaded.
    if (!empty($loaded)) {
      $query->condition('n.nid', $loaded, 'NOT IN');
    }
    // Check if sponsored nodes should not be loaded.
    if ($not_sponsored) {
      $query->innerJoin('field_data_field_sponsored', 'fs', 'fs.entity_id = n.nid');
      $query->condition('fs.field_sponsored_value', 1, '<>');
    }

    // Set term condition if needed.
    if (!empty($tids)) {
      if (!empty($tids) && is_array($tids)) {
        $query->join('taxonomy_index', 'ti', 'ti.nid = n.nid');
        $query->condition('ti.tid', $tids, 'IN');
      }
    }
    // Order by created date.
    $range = 0;
    if (isset($offset)) {
      $range = $offset;
    }
    $query->orderBy('n.created', 'DESC');
    $query->range($range, $quantity);

    $result = $query->execute()->fetchCol();
    if (isset($result)) {
      // Add loaded nids into static cache.
      $loaded = array_merge($loaded, $result);
      // Load multiple nodes.
      $nodes = node_load_multiple($result);
    }

    return $nodes;
  }

  /**
   * Get nodequeue nodes.
   *
   * @param string $name
   *   Nodequeue machine name.
   *
   * @return array|bool|mixed
   *   Array of nodes keyed by nodequeue machine name.
   */
  public static function getNodequeueNodes($name) {
    // Apply static caching.
    $nodequeues = &drupal_static(__FUNCTION__);
    if (!isset($nodequeues[$name])) {
      // Set default nodequeue value.
      $nodequeues[$name] = array();
      // Check if load by name function exists in nodequeue module.
      if (function_exists('nodequeue_load_queue_by_name')) {
        $nodequeue = nodequeue_load_queue_by_name($name);
        if (!empty($nodequeue) && is_object($nodequeue)) {
          $nodes = nodequeue_load_nodes($nodequeue->qid, FALSE, 0, $nodequeue->size);
          // Add node id as key of array because it is required for
          // node_view_multiple().
          $well_indexed_nodes = array();
          if (!empty($nodes) && is_array($nodes)) {
            foreach ($nodes as $node) {
              $well_indexed_nodes[$node->nid] = $node;
            }
          }
          $nodequeues[$name] = $well_indexed_nodes;
        }
        else {
          watchdog('mf_homepage', 'Nodequeue @queue doesn\'t exists, please check machine name.');
        }
      }
      else {
        watchdog('mf_homepage', 'Function nodequeue_load_queue_by_name no longer avaliable in nodequeue module');
      }
    }

    return $nodequeues[$name];
  }

  /**
   * Helper to get nodequeue node ids.
   *
   * @return array
   *   Node ids array.
   */
  protected static function getHomepageNodequeueNodes() {
    $homepage_blocks = mf_homepage_block_info();
    if (!empty($homepage_blocks) && is_array($homepage_blocks)) {
      $homepage_blocks = array_keys($homepage_blocks);
      $nodes = array();
      foreach ($homepage_blocks as $block) {
        $settings = variable_get($block, array());
        if (!empty($settings['nodequeue'])) {
          $nodequeue_nodes = array_keys(self::getNodequeueNodes($settings['nodequeue']));
          // Check if nodequeue is populated with content.
          if (!empty($nodequeue_nodes) && is_array($nodequeue_nodes)) {
            // Use only nodequeues nodes which are displayed in blocks.
            $nodequeue_nodes = array_slice($nodequeue_nodes, 0, $settings['quantity']);
            $nodes = array_merge($nodes, $nodequeue_nodes);
          }
        }
      }
    }

    return $nodes;
  }

  /**
   * Custom helper to get entity field value.
   *
   * @param string $type
   *   Entity type to load.
   * @param object|int|string $entity
   *   Entity id or object.
   * @param string $field
   *   Field name to load.
   *
   * @return mixed
   *   Return field value.
   */
  public static function getEntityFieldValue($type, $entity, $field) {
    $wrapper = entity_metadata_wrapper($type, $entity);
    $property_info = $wrapper->getPropertyInfo();
    if (!empty($property_info[$field])) {
      return $wrapper->{$field}->value();
    }
  }

  /**
   * Check if current node is part of Today's News Nodequeue.
   *
   * @param string|int $nid
   *   Node id.
   *
   * @return bool
   *   Flag to determine node appearance in Today's News nodequeue.
   */
  public static function checkTodaysNews($nid) {
    $nodequeue_nodes = array_keys(self::getNodequeueNodes('todays_news'));
    return in_array($nid, $nodequeue_nodes);
  }

}
